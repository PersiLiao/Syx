/*
   +----------------------------------------------------------------------+
   | Yet Another Framework                                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Xinchen Hui  <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h" /* for zend_alter_ini_entry */
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_application.h"
#include "syx_dispatcher.h"
#include "syx_router.h"
#include "syx_config.h"
#include "syx_loader.h"
#include "syx_request.h"
#include "syx_bootstrap.h"
#include "syx_exception.h"

zend_class_entry * syx_application_ce;

/** {{{ ARG_INFO
 *  */
ZEND_BEGIN_ARG_INFO_EX(syx_application_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_application_construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, config)
	ZEND_ARG_INFO(0, environ)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_application_app_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_application_execute_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_application_getconfig_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_application_getmodule_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_application_getdispatch_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_application_bootstrap_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, bootstrap)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_application_environ_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_application_run_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_application_setappdir_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, directory)
ZEND_END_ARG_INFO()
/* }}} */

int syx_application_is_module_name(zend_string *name) /* {{{ */ {
	zval *modules, *pzval;
	syx_application_t *app;

	app = zend_read_static_property(syx_application_ce, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_APP), 1);
	if (UNEXPECTED(Z_TYPE_P(app) != IS_OBJECT)) {
		return 0;
	}

	modules = zend_read_property(syx_application_ce, app, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_MODULES), 1, NULL);
	if (UNEXPECTED(Z_TYPE_P(modules) != IS_ARRAY)) {
		return 0;
	}

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(modules), pzval) {
		if (UNEXPECTED(Z_TYPE_P(pzval) != IS_STRING)) {
			continue;
		}
		if (zend_string_equals_ci(Z_STR_P(pzval), name)) {
			return 1;
		}
	} ZEND_HASH_FOREACH_END();
	return 0;
}
/* }}} */

int syx_application_is_module_name_str(char *name, size_t len) /* {{{ */ {
	zend_string *str = zend_string_init(name, len, 0);
	int ret = syx_application_is_module_name(str);
	zend_string_release(str);
	return ret;
}
/* }}} */

static int syx_application_parse_option(zval *options) /* {{{ */ {
	HashTable *conf;
	zval *pzval, *psval, *app;

	conf = HASH_OF(options);
	if (UNEXPECTED((app = zend_hash_str_find(conf, ZEND_STRL("application"))) == NULL)) {
		/* For back compatibilty */
		if ((app = zend_hash_str_find(conf, ZEND_STRL("syx"))) == NULL) {
			syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s", "Expected an array of application configure");
			return FAILURE;
		}
	}

	if (UNEXPECTED(Z_TYPE_P(app) != IS_ARRAY)) {
		syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s", "Expected an array of application configure");
		return FAILURE;
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(app),
					ZEND_STRL("directory"))) == NULL || Z_TYPE_P(pzval) != IS_STRING || Z_STRLEN_P(pzval) == 0)) {
		syx_trigger_error(SYX_ERR_STARTUP_FAILED, "%s", "Expected a directory entry in application configures");
		return FAILURE;
	}

	if (UNEXPECTED(*(Z_STRVAL_P(pzval) + Z_STRLEN_P(pzval) - 1) == DEFAULT_SLASH)) {
		SYX_G(directory) = zend_string_init(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval) - 1, 0);
	} else {
		SYX_G(directory) = zend_string_copy(Z_STR_P(pzval));
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(app),
						ZEND_STRL("ext"))) != NULL && Z_TYPE_P(pzval) == IS_STRING)) {
		SYX_G(ext) = zend_string_copy(Z_STR_P(pzval));
	}

	// TODO 2 添加namespace命名空间
	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(app),
							ZEND_STRL("namespace"))) != NULL && Z_TYPE_P(pzval) == IS_STRING)) {
		SYX_G(namespace) = zend_string_copy(Z_STR_P(pzval));
	}else{
		SYX_G(namespace) = zend_string_init(SYX_APPLICATION_NAMESPACE, sizeof(SYX_APPLICATION_NAMESPACE) -1, 0);
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(app),
						ZEND_STRL("bootstrap"))) != NULL && Z_TYPE_P(pzval) == IS_STRING)) {
		SYX_G(bootstrap) = zend_string_copy(Z_STR_P(pzval));
	}

	if (EXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(app), ZEND_STRL("library"))) != NULL)) {
		if (IS_STRING == Z_TYPE_P(pzval)) {
			if (*(Z_STRVAL_P(pzval) + Z_STRLEN_P(pzval)) == DEFAULT_SLASH) {
				SYX_G(local_library) = zend_string_init(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval) - 1, 0);
			} else {
				SYX_G(local_library) = zend_string_copy(Z_STR_P(pzval));
			}
		} else if (IS_ARRAY == Z_TYPE_P(pzval)) {
			if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval),
							ZEND_STRL("directory"))) != NULL && Z_TYPE_P(psval) == IS_STRING) {
				if (*(Z_STRVAL_P(psval) + Z_STRLEN_P(psval)) == DEFAULT_SLASH) {
					SYX_G(local_library) = zend_string_init(Z_STRVAL_P(psval), Z_STRLEN_P(psval) - 1, 0);
				} else {
					SYX_G(local_library) = zend_string_copy(Z_STR_P(psval));
				}
			}
			if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval),
							ZEND_STRL("namespace"))) != NULL && Z_TYPE_P(psval) == IS_STRING) {
				uint i, len;
				char *src = Z_STRVAL_P(psval);
				if (Z_STRLEN_P(psval)) {
				    char *target = emalloc(Z_STRLEN_P(psval) + 1);
					len = 0;
					for(i=0; i<Z_STRLEN_P(psval); i++) {
						if (src[i] == ',') {
							target[len++] = DEFAULT_DIR_SEPARATOR;
						} else if (src[i] != ' ') {
							target[len++] = src[i];
						}
					}
					target[len] = '\0';
					syx_loader_register_namespace_single(target, len);
					efree(target);
				}
			}
		}
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(app),
						ZEND_STRL("view"))) != NULL && Z_TYPE_P(pzval) == IS_ARRAY)) {
		if (UNEXPECTED((psval = zend_hash_str_find(Z_ARRVAL_P(pzval),
							ZEND_STRL("ext"))) != NULL && Z_TYPE_P(psval) == IS_STRING)) {
			zend_string_release(SYX_G(view_ext));
			SYX_G(view_ext) = zend_string_copy(Z_STR_P(psval));
		}
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(app),
						ZEND_STRL("baseUri"))) != NULL && Z_TYPE_P(pzval) == IS_STRING)) {
		SYX_G(base_uri) = zend_string_copy(Z_STR_P(pzval));
	}

	if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(app),
						ZEND_STRL("dispatcher"))) != NULL && Z_TYPE_P(pzval) == IS_ARRAY)) {
		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval),
						ZEND_STRL("defaultModule"))) != NULL && Z_TYPE_P(psval) == IS_STRING) {
			zend_string_release(SYX_G(default_module));
			SYX_G(default_module) = zend_string_dup(Z_STR_P(psval), 0);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval),
						ZEND_STRL("defaultController"))) != NULL && Z_TYPE_P(psval) == IS_STRING) {
			zend_string_release(SYX_G(default_controller));
			SYX_G(default_controller) = zend_string_dup(Z_STR_P(psval), 0);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval),
						ZEND_STRL("defaultAction"))) != NULL && Z_TYPE_P(psval) == IS_STRING) {
			zend_string_release(SYX_G(default_action));
			SYX_G(default_action) = zend_string_tolower(Z_STR_P(psval));
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("throwException"))) != NULL) {
			SYX_G(throw_exception) = zend_is_true(psval);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval), ZEND_STRL("catchException"))) != NULL) {
			SYX_G(catch_exception) = zend_is_true(psval);
		}

		if ((psval = zend_hash_str_find(Z_ARRVAL_P(pzval),
						ZEND_STRL("defaultRoute"))) != NULL && Z_TYPE_P(psval) == IS_ARRAY) {
			/* increase the refcount? */
			SYX_G(default_route) = psval;
		}
	}

	do {
		char *ptrptr;
		zval module;

		array_init(&SYX_G(modules));
		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app),
						ZEND_STRL("modules"))) != NULL && Z_TYPE_P(pzval) == IS_STRING && Z_STRLEN_P(pzval)) {
			char *seg, *modules;
			modules = estrndup(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
			seg = php_strtok_r(modules, ",", &ptrptr);
			while (seg) {
				if (seg && strlen(seg)) {
					ZVAL_STRINGL(&module, seg, strlen(seg));
					zend_hash_next_index_insert(Z_ARRVAL(SYX_G(modules)), &module);
				}
				seg = php_strtok_r(NULL, ",", &ptrptr);
			}
			efree(modules);
		} else {
			ZVAL_STR_COPY(&module, SYX_G(default_module));
			zend_hash_next_index_insert(Z_ARRVAL(SYX_G(modules)), &module);
		}
	} while (0);

	if ((pzval = zend_hash_str_find(Z_ARRVAL_P(app),
					ZEND_STRL("system"))) != NULL && Z_TYPE_P(pzval) == IS_ARRAY) {
		zval *value;
		char name[128];
		zend_string *key;
		size_t len;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pzval), key, value) {
			zend_string *str, *val;

			len = snprintf(name, sizeof(name), "%s.%s", "syx", ZSTR_VAL(key));
			if (len > sizeof(name) -1) {
				len = sizeof(name) - 1;
			}
			str = zend_string_init(name, len, 0);
			val = zval_get_string(value);
			zend_alter_ini_entry(str, val, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
			zend_string_release(str);
			zend_string_release(val);
		} ZEND_HASH_FOREACH_END();
	}

	return SUCCESS;
}
/* }}} */

/** {{{ proto Syx_Application::__construct(mixed $config, string $environ = SYX_G(environ_name))
*/
PHP_METHOD(syx_application, __construct) {
	zval *config;
	zval *section = NULL, zsection = {{0}};
	syx_config_t zconfig = {{0}};
	syx_request_t zrequest = {{0}};
	syx_dispatcher_t zdispatcher = {{0}};
	syx_application_t *app, *self;
	syx_loader_t *loader, zloader = {{0}};

#if PHP_SYX_DEBUG
	php_error_docref(NULL, E_STRICT, "Syx is running in debug mode");
#endif

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z|z", &config, &section) == FAILURE) {
		return;
	}

	app	= zend_read_static_property(syx_application_ce, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_APP), 1);

	if (!ZVAL_IS_NULL(app)) {
		syx_trigger_error(SYX_ERR_STARTUP_FAILED, "Only one application can be initialized");
		RETURN_FALSE;
	}

	self = getThis();
	if (!section || Z_TYPE_P(section) != IS_STRING || !Z_STRLEN_P(section)) {
		ZVAL_STRING(&zsection, SYX_G(environ_name));
		(void)syx_config_instance(&zconfig, config, &zsection);
		zval_ptr_dtor(&zsection);
	} else {
		SYX_G(environ_name) = Z_STRVAL_P(section); // TODO 1 修复Application 返回environ值问题
		(void)syx_config_instance(&zconfig, config, section);
	}

	if  (UNEXPECTED(Z_TYPE(zconfig) != IS_OBJECT
			|| syx_application_parse_option(zend_read_property(syx_config_ce,
					&zconfig, ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL)) == FAILURE)) {
		syx_trigger_error(SYX_ERR_STARTUP_FAILED, "Initialization of application config failed");
		zval_ptr_dtor(&zconfig);
		RETURN_FALSE;
	}

	(void)syx_request_instance(&zrequest, SYX_G(base_uri));

	if (UNEXPECTED(Z_TYPE(zrequest) != IS_OBJECT)) {
		syx_trigger_error(SYX_ERR_STARTUP_FAILED, "Initialization of request failed");
		RETURN_FALSE;
	}

	(void)syx_dispatcher_instance(&zdispatcher);
	if (UNEXPECTED(Z_TYPE(zdispatcher) != IS_OBJECT)) {
		syx_trigger_error(SYX_ERR_STARTUP_FAILED, "Instantiation of application dispatcher failed");
		RETURN_FALSE;
	}
	syx_dispatcher_set_request(&zdispatcher, &zrequest);
	zval_ptr_dtor(&zrequest);

	zend_update_property(syx_application_ce, self, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_CONFIG), &zconfig);
	zend_update_property(syx_application_ce, self, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_DISPATCHER), &zdispatcher);

	zval_ptr_dtor(&zdispatcher);
	zval_ptr_dtor(&zconfig);

	if (SYX_G(local_library)) {
		zend_string *global_library = strlen(SYX_G(global_library))?
			zend_string_init(SYX_G(global_library), strlen(SYX_G(global_library)), 0) : NULL;
		loader = syx_loader_instance(&zloader, SYX_G(local_library), global_library);
		if (global_library) {
			zend_string_release(global_library);
		}
	} else {
		zend_string *local_library, *global_library;
		local_library = strpprintf(0, "%s%c%s", ZSTR_VAL(SYX_G(directory)), DEFAULT_SLASH, SYX_LIBRARY_DIRECTORY_NAME);
		global_library = strlen(SYX_G(global_library))?
			zend_string_init(SYX_G(global_library), strlen(SYX_G(global_library)), 0) : NULL;
		loader = syx_loader_instance(&zloader, local_library, global_library);
		zend_string_release(local_library);
		if (global_library) {
			zend_string_release(global_library);
		}
	}

	if (UNEXPECTED(Z_TYPE_P(loader) != IS_OBJECT)) {
		syx_trigger_error(SYX_ERR_STARTUP_FAILED, "Initialization of application auto loader failed");
		RETURN_FALSE;
	}

	zend_update_property_bool(syx_application_ce, self, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_RUN), 0);
	zend_update_property_string(syx_application_ce, self, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ENV), SYX_G(environ_name));

	if (Z_TYPE(SYX_G(modules)) == IS_ARRAY) {
		zend_update_property(syx_application_ce,
				self, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_MODULES), &SYX_G(modules));
	} else {
		zend_update_property_null(syx_application_ce, self, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_MODULES));
	}

	zend_update_static_property(syx_application_ce, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_APP), self);
}
/* }}} */

/** {{{ proto public Syx_Application::__desctruct(void)
*/
PHP_METHOD(syx_application, __destruct) {
	zend_update_static_property_null(syx_application_ce, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_APP));
}
/* }}} */

/** {{{ proto private Syx_Application::__sleep(void)
*/
PHP_METHOD(syx_application, __sleep) {
}
/* }}} */

/** {{{ proto private Syx_Application::__wakeup(void)
*/
PHP_METHOD(syx_application, __wakeup) {
}
/* }}} */

/** {{{ proto private Syx_Application::__clone(void)
*/
PHP_METHOD(syx_application, __clone) {
}
/* }}} */

/** {{{ proto public Syx_Application::run(void)
*/
PHP_METHOD(syx_application, run) {
	zval              *running;
	syx_dispatcher_t  *dispatcher;
	syx_response_t	  *response, rresponse;
	syx_application_t *self = getThis();

	running = zend_read_property(syx_application_ce, self,
			ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_RUN), 1, NULL);
	if (UNEXPECTED(IS_TRUE == Z_TYPE_P(running))) {
		syx_trigger_error(SYX_ERR_STARTUP_FAILED, "An application instance already run");
		RETURN_TRUE;
	}

	ZVAL_BOOL(running, 1);

	dispatcher = zend_read_property(syx_application_ce, self,
			ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_DISPATCHER), 1, NULL);
	ZVAL_NULL(&rresponse);
	if ((response = syx_dispatcher_dispatch(dispatcher, &rresponse))) {
		RETURN_ZVAL(response, 1, 1);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Application::execute(callback $func)
 * We can not call to zif_call_user_func on windows, since it was not declared with dllexport
 */
PHP_METHOD(syx_application, execute) {
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "f*", &fci, &fci_cache, &fci.params, &fci.param_count) == FAILURE) {
		return;
	}

	fci.retval = &retval;

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		ZVAL_COPY_VALUE(return_value, &retval);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Syx_Application::app(void)
*/
PHP_METHOD(syx_application, app) {
	syx_application_t *app = zend_read_static_property(syx_application_ce,
		   	ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_APP), 1);
	RETURN_ZVAL(app, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Application::getConfig(void)
*/
PHP_METHOD(syx_application, getConfig) {
	syx_config_t *config = zend_read_property(syx_application_ce,
			getThis(), ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_CONFIG), 1, NULL);
	RETURN_ZVAL(config, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Application::getDispatcher(void)
*/
PHP_METHOD(syx_application, getDispatcher) {
	syx_dispatcher_t *dispatcher = zend_read_property(syx_application_ce,
			getThis(), ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_DISPATCHER), 1, NULL);
	RETURN_ZVAL(dispatcher, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Application::getModules(void)
*/
PHP_METHOD(syx_application, getModules) {
	zval *modules = zend_read_property(syx_application_ce,
			getThis(), ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_MODULES), 1, NULL);
	RETURN_ZVAL(modules, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Application::environ(void)
*/
PHP_METHOD(syx_application, environ) {
	zval *env = zend_read_property(syx_application_ce,
			getThis(), ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ENV), 1, NULL);
	ZEND_ASSERT(Z_TYPE_P(env) == IS_STRING);
	RETURN_STR(zend_string_copy(Z_STR_P(env)));
}
/* }}} */

/** {{{ proto public Syx_Application::bootstrap(void)
*/
PHP_METHOD(syx_application, bootstrap) {
	zend_string	*bootstrap_path;
	uint  retval = 1;
	zend_class_entry  *ce;
	syx_application_t *self = getThis();

	if (!(ce = zend_hash_str_find_ptr(EG(class_table),
					SYX_DEFAULT_BOOTSTRAP_LOWER, sizeof(SYX_DEFAULT_BOOTSTRAP_LOWER) - 1))) {
		if (SYX_G(bootstrap)) {
			bootstrap_path = zend_string_copy(SYX_G(bootstrap));
		} else {
			bootstrap_path = strpprintf(0, "%s%c%s.%s",
					ZSTR_VAL(SYX_G(directory)), DEFAULT_SLASH, SYX_DEFAULT_BOOTSTRAP, ZSTR_VAL(SYX_G(ext)));
		}
		if (!syx_loader_import(bootstrap_path, 0)) {
			php_error_docref(NULL, E_WARNING, "Couldn't find bootstrap file %s", ZSTR_VAL(bootstrap_path));
			retval = 0;
		} else if (UNEXPECTED((ce = zend_hash_str_find_ptr(EG(class_table),
						SYX_DEFAULT_BOOTSTRAP_LOWER, sizeof(SYX_DEFAULT_BOOTSTRAP_LOWER) - 1)) == NULL)) {
			php_error_docref(NULL, E_WARNING, "Couldn't find class %s in %s", SYX_DEFAULT_BOOTSTRAP, ZSTR_VAL(bootstrap_path));
			retval = 0;
		} else if (UNEXPECTED(!instanceof_function(ce, syx_bootstrap_ce))) {
			php_error_docref(NULL, E_WARNING,
					"Expect a %s instance, %s give", ZSTR_VAL(syx_bootstrap_ce->name), ZSTR_VAL(ce->name));
			retval = 0;
		}
		zend_string_release(bootstrap_path);
	}

	if (UNEXPECTED(!retval)) {
		RETURN_FALSE;
	} else {
		zend_string *func;
		zval bootstrap;
		syx_dispatcher_t *dispatcher;

		object_init_ex(&bootstrap, ce);
		dispatcher = zend_read_property(syx_application_ce,
				self, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_DISPATCHER), 1, NULL);

		ZEND_HASH_FOREACH_STR_KEY(&(ce->function_table), func) {
			/* cann't use ZEND_STRL in strncasecmp, it cause a compile failed in VS2009 */
			if (strncasecmp(ZSTR_VAL(func), SYX_BOOTSTRAP_INITFUNC_PREFIX, sizeof(SYX_BOOTSTRAP_INITFUNC_PREFIX)-1)) {
				continue;
			}
			zend_call_method(&bootstrap, ce, NULL, ZSTR_VAL(func), ZSTR_LEN(func), NULL, 1, dispatcher, NULL);
			/** an uncaught exception threw in function call */
			if (UNEXPECTED(EG(exception))) {
				zval_ptr_dtor(&bootstrap);
				RETURN_FALSE;
			}
		} ZEND_HASH_FOREACH_END();
		zval_ptr_dtor(&bootstrap);
	}

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Application::getLastErrorNo(void)
*/
PHP_METHOD(syx_application, getLastErrorNo) {
	zval *errcode = zend_read_property(syx_application_ce,
			getThis(), ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ERRNO), 1, NULL);
	ZEND_ASSERT(Z_TYPE_P(errcode) == IS_LONG);
	RETURN_LONG(Z_LVAL_P(errcode));
}
/* }}} */

/** {{{ proto public Syx_Application::getLastErrorMsg(void)
*/
PHP_METHOD(syx_application, getLastErrorMsg) {
	zval *errmsg = zend_read_property(syx_application_ce,
			getThis(), ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ERRMSG), 1, NULL);
	ZEND_ASSERT(Z_TYPE_P(errmsg) == IS_STRING);
	RETURN_STR(zend_string_copy(Z_STR_P(errmsg)));
}
/* }}} */

/** {{{ proto public Syx_Application::clearLastError(void)
*/
PHP_METHOD(syx_application, clearLastError) {
	zend_update_property_long(syx_application_ce,
			getThis(), ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ERRNO), 0);
	zend_update_property_string(syx_application_ce,
			getThis(), ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ERRMSG), "");
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Application::setAppDirectory(string $directory)
*/
PHP_METHOD(syx_application, setAppDirectory) {
	zend_string      *directory;
	syx_application_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &directory) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(directory) == 0 || !IS_ABSOLUTE_PATH(ZSTR_VAL(directory), ZSTR_LEN(directory))) {
		RETURN_FALSE;
	}

	zend_string_release(SYX_G(directory));
	SYX_G(directory) = zend_string_copy(directory);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Application::getAppDirectory(void)
*/
PHP_METHOD(syx_application, getAppDirectory) {
	RETURN_STR(zend_string_copy(SYX_G(directory)));
}
/* }}} */

/** {{{ syx_application_methods
*/
zend_function_entry syx_application_methods[] = {
	PHP_ME(syx_application, __construct,         syx_application_construct_arginfo,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(syx_application, run,                 syx_application_run_arginfo,         ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, execute,             syx_application_execute_arginfo,     ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, app,                 syx_application_app_arginfo,         ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	SYX_ME(syx_application_environ, "environ",   syx_application_environ_arginfo,     ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, bootstrap,           syx_application_bootstrap_arginfo,      ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, getConfig,           syx_application_getconfig_arginfo,     ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, getModules,          syx_application_getmodule_arginfo,      ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, getDispatcher,       syx_application_getdispatch_arginfo,    ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, setAppDirectory,     syx_application_setappdir_arginfo,      ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, getAppDirectory,     syx_application_void_arginfo,         ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, getLastErrorNo,      syx_application_void_arginfo,         ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, getLastErrorMsg,     syx_application_void_arginfo,         ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, clearLastError,      syx_application_void_arginfo,         ZEND_ACC_PUBLIC)
	PHP_ME(syx_application, __destruct,          NULL,                     ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	PHP_ME(syx_application, __clone,             NULL,                     ZEND_ACC_PRIVATE)
	PHP_ME(syx_application, __sleep,             NULL,                     ZEND_ACC_PRIVATE)
	PHP_ME(syx_application, __wakeup,            NULL,                     ZEND_ACC_PRIVATE)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(application) {
	zend_class_entry ce;
	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Application", syx_application_methods);

	syx_application_ce = zend_register_internal_class_ex(&ce, NULL);
	syx_application_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(syx_application_ce,
			ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_CONFIG), ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_application_ce,
			ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_DISPATCHER), ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_application_ce,
			ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_APP), ZEND_ACC_STATIC | ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_application_ce,
			ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_MODULES), ZEND_ACC_PROTECTED);

	zend_declare_property_bool(syx_application_ce,
			ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_RUN), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_string(syx_application_ce,
			ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ENV), SYX_G(environ_name), ZEND_ACC_PROTECTED);

	zend_declare_property_long(syx_application_ce,
			ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ERRNO), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_string(syx_application_ce,
			ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ERRMSG), "", ZEND_ACC_PROTECTED);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
