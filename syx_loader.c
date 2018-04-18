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
#include "zend_smart_str.h" /* for smart_str */

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_application.h"
#include "syx_request.h"
#include "syx_loader.h"
#include "syx_exception.h"

zend_class_entry *syx_loader_ce;

/** {{{ ARG_INFO
 */
SYX_BEGIN_ARG_INFO_EX(syx_loader_void_arginfo, 0, 0, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_loader_getinstance_arginfo, 0, 0, 0)
    SYX_ARG_INFO(0, local_library_path)
    SYX_ARG_INFO(0, global_library_path)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_loader_autoloader_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, class_name)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_loader_regnamespace_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, name_prefix)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_loader_islocalname_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, class_name)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_loader_import_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, file)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_loader_setlib_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, library_path)
    SYX_ARG_INFO(0, is_global)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_loader_getlib_arginfo, 0, 0, 0)
    SYX_ARG_INFO(0, is_global)
SYX_END_ARG_INFO()
/* }}} */

int syx_loader_register(syx_loader_t *loader) /* {{{ */ {
	zval autoload, function, method, ret;

	array_init(&autoload);

    ZVAL_STRING(&method, SYX_AUTOLOAD_FUNC_NAME);
	zend_hash_next_index_insert(Z_ARRVAL(autoload), loader);
	zend_hash_next_index_insert(Z_ARRVAL(autoload), &method);

	ZVAL_STRING(&function, SYX_SPL_AUTOLOAD_REGISTER_NAME);

	do {
		zend_fcall_info fci = {
			sizeof(fci),
#if PHP_VERSION_ID < 70100
			EG(function_table),
#endif
			function,
#if PHP_VERSION_ID < 70100
			NULL,
#endif
			&ret,
			&autoload,
			NULL,
			1,
			1
		};

		if (zend_call_function(&fci, NULL) == FAILURE) {
			zval_ptr_dtor(&function);
			zval_ptr_dtor(&autoload);
			php_error_docref(NULL,
					E_WARNING,
					"Unable to register autoload function %s",
					SYX_AUTOLOAD_FUNC_NAME);
			return 0;
		}
		zval_ptr_dtor(&function);
		zval_ptr_dtor(&autoload);
	} while (0);
	return 1;
}
/* }}} */

// TODO 4 添加syx_loader_is_app
static int syx_loader_is_app(char *class_name, size_t class_len){

	if(class_len > ZSTR_LEN(SYX_G(namespace))){
	    char *class_ns = estrndup(class_name, ZSTR_LEN(SYX_G(namespace)));
		if(strcmp(class_ns, ZSTR_VAL(SYX_G(namespace)) ) == 0){
		    efree(class_ns);
			return 1;
		}
		efree(class_ns);
	}
	return 0;
}

/** {{{ int syx_loader_is_local_namespace(syx_loader_t *loader, char *class_name, int len)
 */
int syx_loader_is_local_namespace(syx_loader_t *loader, char *class_name, int len) {
	char *pos, *ns, *prefix;
	char orig_char = 0, *backup = NULL;
	size_t prefix_len;

	if (!SYX_G(local_namespaces)) {
		return 0;
	}

	ns = ZSTR_VAL(SYX_G(local_namespaces));

	pos = strstr(class_name, "_");
	if (pos) {
		prefix_len 	= pos - class_name;
		prefix 	= class_name;
		backup = class_name + prefix_len;
		orig_char = '_';
		*backup = '\0';
	} else if ((pos = strstr(class_name, "\\"))) {
		prefix_len 	= pos - class_name;
		prefix 	= estrndup(class_name, prefix_len);
		orig_char = '\\';
		backup = class_name + prefix_len;
		*backup = '\0';
	} else {
		prefix = class_name;
		prefix_len = len;
	}

	while ((pos = strstr(ns, prefix))) {
		if ((pos == ns) && (*(pos + prefix_len) == DEFAULT_DIR_SEPARATOR || *(pos + prefix_len) == '\0')) {
			if (backup) {
				*backup = orig_char;
			}
			if (prefix != class_name) {
				efree(prefix);
			}
			return 1;
		} else if (*(pos - 1) == DEFAULT_DIR_SEPARATOR
				&& (*(pos + prefix_len) == DEFAULT_DIR_SEPARATOR || *(pos + prefix_len) == '\0')) {
			if (backup) {
				*backup = orig_char;
			}
			if (prefix != class_name) {
				efree(prefix);
			}
			return 1;
		}
		ns = pos + prefix_len;
	}

	if (backup) {
		*backup = orig_char;
	}

	if (prefix != class_name) {
		efree(prefix);
	}

	return 0;
}
/* }}} */

syx_loader_t *syx_loader_instance(syx_loader_t *this_ptr, zend_string *library_path, zend_string *global_path) /* {{{ */ {
	syx_loader_t *instance;

	instance = zend_read_static_property(syx_loader_ce, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_INSTANCE), 1);

	if (IS_OBJECT == Z_TYPE_P(instance)) {
		if (library_path) {
			zend_update_property_str(syx_loader_ce, instance, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_LIBRARY), library_path);
		}

		if (global_path) {
			zend_update_property_str(syx_loader_ce, instance, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_GLOBAL_LIB), global_path);
		}
		return instance;
	}

	if (!global_path && !library_path) {
		php_error_docref(NULL, E_WARNING, "Missed library directory arguments");
		return NULL;
	}

	if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, syx_loader_ce);
	}

	if (library_path && global_path) {
		zend_update_property_str(syx_loader_ce, this_ptr, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_LIBRARY), library_path);
		zend_update_property_str(syx_loader_ce, this_ptr, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_GLOBAL_LIB), global_path);
	} else if (!global_path) {
		zend_update_property_str(syx_loader_ce, this_ptr, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_LIBRARY), library_path);
		zend_update_property_str(syx_loader_ce, this_ptr, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_GLOBAL_LIB), library_path);
	} else {
		zend_update_property_str(syx_loader_ce, this_ptr, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_LIBRARY), global_path);
		zend_update_property_str(syx_loader_ce, this_ptr, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_GLOBAL_LIB), global_path);
	}

	if (!syx_loader_register(this_ptr)) {
		php_error_docref(NULL, E_WARNING, "Failed to register autoload function");
	}

	zend_update_static_property(syx_loader_ce, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_INSTANCE), this_ptr);

	return this_ptr;
}
/* }}} */

/** {{{ int syx_loader_import(zend_string *path, int use_path)
*/
int syx_loader_import(zend_string *path, int use_path) {
	zend_file_handle file_handle;
	zend_op_array 	*op_array;
	char realpath[MAXPATHLEN];

	if (!VCWD_REALPATH(ZSTR_VAL(path), realpath)) {
		return 0;
	}

	file_handle.filename = ZSTR_VAL(path);
	file_handle.free_filename = 0;
	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.opened_path = NULL;
	file_handle.handle.fp = NULL;

	op_array = zend_compile_file(&file_handle, ZEND_INCLUDE);
	if (op_array && file_handle.handle.stream.handle) {
		if (!file_handle.opened_path) {
			file_handle.opened_path = zend_string_copy(path);
		}

		zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path);
	}
	zend_destroy_file_handle(&file_handle);
	if (op_array) {
		zval result;

        ZVAL_UNDEF(&result);
		zend_execute(op_array, &result);

		destroy_op_array(op_array);
		efree(op_array);
        if (!EG(exception)) {
            zval_ptr_dtor(&result);
        }

	    return 1;
	}

	return 0;
}
/* }}} */

int syx_internal_autoload(char *file_name, size_t name_len, char **directory) /* {{{ */ {
	int status;
	char *q, *p;
	size_t directory_len;
	smart_str buf = {0};

	if (NULL == *directory) {
		zend_string *library_path;
		syx_loader_t *loader, rv = {{0}};

		loader = syx_loader_instance(&rv, NULL, NULL);

		if (loader == NULL) {
			/* since only call from userspace can cause loader is NULL, exception throw will works well */
			php_error_docref(NULL, E_WARNING, "%s need to be initialize first", ZSTR_VAL(syx_loader_ce->name));
			return 0;
		} else {
			zval *library_dir;

			if (syx_loader_is_local_namespace(loader, file_name, name_len)) {
				library_dir = zend_read_property(syx_loader_ce, loader, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_LIBRARY), 1, NULL);
			} else {
				library_dir	= zend_read_property(syx_loader_ce, loader, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_GLOBAL_LIB), 1, NULL);
			}

			library_path = Z_STR_P(library_dir);
		}

		smart_str_appendl(&buf, ZSTR_VAL(library_path), ZSTR_LEN(library_path));

	} else {
		smart_str_appendl(&buf, *directory, strlen(*directory));
		efree(*directory);
	}

	directory_len = ZSTR_LEN(buf.s);

	/* aussume all the path is not end in slash */
	smart_str_appendc(&buf, DEFAULT_SLASH);

	p = file_name;
	q = p;

	/* Syx_Application => Syx/Application */
	while (1) {
		do {
			q++;
		} while (*q != '_' && *q != '\0');
		if (*q != '\0') {
			smart_str_appendl(&buf, p, q - p);
			smart_str_appendc(&buf, DEFAULT_SLASH);
			p = q + 1;
		} else {
			break;
		}
	}

	if (SYX_G(lowcase_path)) {
		/* all path of library is lowercase */
		zend_str_tolower(ZSTR_VAL(buf.s) + directory_len, ZSTR_LEN(buf.s) - directory_len);
	}
	smart_str_appendl(&buf, p, strlen(p));
	smart_str_appendc(&buf, '.');
	smart_str_appendl(&buf, ZSTR_VAL(SYX_G(ext)), ZSTR_LEN(SYX_G(ext)));

	smart_str_0(&buf);

	if (directory) {
		*(directory) = estrndup(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
	}
	status = syx_loader_import(buf.s, 0);
	smart_str_free(&buf);

	return status;
}
/* }}} */

int syx_loader_register_namespace_single(char *prefix, size_t len) /* {{{ */ {

	if (SYX_G(local_namespaces)) {
		SYX_G(local_namespaces) = zend_string_realloc(
				SYX_G(local_namespaces), ZSTR_LEN(SYX_G(local_namespaces)) + len + 1, 0);
		snprintf(ZSTR_VAL(SYX_G(local_namespaces)) +
				ZSTR_LEN(SYX_G(local_namespaces)) - (len + 1), len + 1 + 1, "%c%s", DEFAULT_DIR_SEPARATOR, prefix);
	} else {
		SYX_G(local_namespaces) = zend_string_init(prefix, len, 0);
	}

	return 1;
}
/* }}} */

/** {{{ int syx_loader_register_namespace_multi(zval *prefixes)
 */
int syx_loader_register_namespace_multi(zval *prefixes) {
	zval *pzval;
	HashTable *ht;

	ht = Z_ARRVAL_P(prefixes);
	ZEND_HASH_FOREACH_VAL(ht, pzval) {
		if (IS_STRING == Z_TYPE_P(pzval)) {
			syx_loader_register_namespace_single(Z_STRVAL_P(pzval), Z_STRLEN_P(pzval));
		}
	} ZEND_HASH_FOREACH_END();

	return 1;
}
/* }}} */

/** {{{ proto private Syx_Loader::__construct(void)
*/
PHP_METHOD(syx_loader, __construct) {
}
/* }}} */

/** {{{ proto private Syx_Loader::__sleep(void)
*/
PHP_METHOD(syx_loader, __sleep) {
}
/* }}} */

/** {{{ proto private Syx_Loader::__wakeup(void)
*/
PHP_METHOD(syx_loader, __wakeup) {
}
/* }}} */

/** {{{ proto private Syx_Loader::__clone(void)
*/
PHP_METHOD(syx_loader, __clone) {
}
/* }}} */

/** {{{ proto public Syx_Loader::registerLocalNamespace(mixed $namespace)
*/
PHP_METHOD(syx_loader, registerLocalNamespace) {
	zval *namespaces;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &namespaces) == FAILURE) {
		return;
	}

	if (IS_STRING == Z_TYPE_P(namespaces)) {
		if (syx_loader_register_namespace_single(Z_STRVAL_P(namespaces), Z_STRLEN_P(namespaces))) {
			RETURN_ZVAL(getThis(), 1, 0);
		}
	} else if (IS_ARRAY == Z_TYPE_P(namespaces)) {
		if(syx_loader_register_namespace_multi(namespaces)) {
			RETURN_ZVAL(getThis(), 1, 0);
		}
	} else {
		php_error_docref(NULL, E_WARNING, "Invalid parameters provided, must be a string, or an array");
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Loader::getLocalNamespace(void)
*/
PHP_METHOD(syx_loader, getLocalNamespace) {
	if (SYX_G(local_namespaces)) {
		RETURN_STR(zend_string_copy(SYX_G(local_namespaces)));
	}
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Syx_Loader::clearLocalNamespace(void)
*/
PHP_METHOD(syx_loader, clearLocalNamespace) {
	zend_string_release(SYX_G(local_namespaces));
	SYX_G(local_namespaces) = NULL;
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Loader::isLocalName(string $class_name)
*/
PHP_METHOD(syx_loader, isLocalName) {
	zval *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &name) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(name) != IS_STRING) {
		RETURN_FALSE;
	}

	RETURN_BOOL(syx_loader_is_local_namespace(getThis(), Z_STRVAL_P(name), Z_STRLEN_P(name)));
}
/* }}} */

/** {{{ proto public Syx_Loader::setLibraryPath(string $path, $global = FALSE)
*/
PHP_METHOD(syx_loader, setLibraryPath) {
	zend_string *library;
	zend_bool global = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &library, &global) == FAILURE) {
		return;
	}

	if (!global) {
		zend_update_property_str(syx_loader_ce, getThis(), ZEND_STRL(SYX_LOADER_PROPERTY_NAME_LIBRARY), library);
	} else {
		zend_update_property_str(syx_loader_ce, getThis(), ZEND_STRL(SYX_LOADER_PROPERTY_NAME_GLOBAL_LIB), library);
	}

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Loader::getLibraryPath($global = FALSE)
*/
PHP_METHOD(syx_loader, getLibraryPath) {
	zval *library;
	zend_bool global = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &global) == FAILURE) {
		return;
	}

	if (!global) {
		library = zend_read_property(syx_loader_ce, getThis(), ZEND_STRL(SYX_LOADER_PROPERTY_NAME_LIBRARY), 1, NULL);
	} else {
		library = zend_read_property(syx_loader_ce, getThis(), ZEND_STRL(SYX_LOADER_PROPERTY_NAME_GLOBAL_LIB), 1, NULL);
	}

	RETURN_ZVAL(library, 1, 0);
}
/* }}} */

/** {{{ proto public static Syx_Loader::import($file)
*/
PHP_METHOD(syx_loader, import) {
	zend_string *file;
	int need_free = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &file) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(file) == 0) {
		RETURN_FALSE;
	} else {
		int retval;
		syx_loader_t *loader, rv = {{0}};

		if (!IS_ABSOLUTE_PATH(ZSTR_VAL(file), ZSTR_LEN(file))) {
			loader = syx_loader_instance(&rv, NULL, NULL);
			if (loader == NULL) {
				php_error_docref(NULL, E_WARNING, "%s need to be initialize first", ZSTR_VAL(syx_loader_ce->name));
				RETURN_FALSE;
			} else {
				zval *library = zend_read_property(syx_loader_ce,
						loader, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_LIBRARY), 1, NULL);
				file = strpprintf(0, "%s%c%s", Z_STRVAL_P(library), DEFAULT_SLASH, ZSTR_VAL(file));
				need_free = 1;
			}
		}

		retval = zend_hash_exists(&EG(included_files), file);
		if (retval) {
			if (need_free) {
				zend_string_release(file);
			}
			RETURN_TRUE;
		}

		retval = syx_loader_import(file, 0);
		if (need_free) {
			zend_string_release(file);
		}

		RETURN_BOOL(retval);
	}
}
/* }}} */

/** {{{ proto public Syx_Loader::autoload($class_name)
*/
PHP_METHOD(syx_loader, autoload) {
	char *class_name, *origin_classname, *app_directory, *directory = NULL, *file_name = NULL;
	char *dup_lcname = NULL;
	size_t file_name_len = 0;
	size_t class_name_len;
	zend_bool ret = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &class_name, &class_name_len) == FAILURE) {
		return;
	}

	app_directory = SYX_G(directory)? ZSTR_VAL(SYX_G(directory)) : NULL;
	origin_classname = class_name;

	do {
		if (!class_name_len) {
			break;
		} else {
			char *pos;
			if ((pos = strchr(class_name, '\\')) != NULL) {
				dup_lcname = estrndup(class_name, class_name_len);
				pos = dup_lcname + (pos - class_name);
				*pos = '_';
				while (*(++pos) != '\0') {
					if (*pos == '\\') {
						*pos = '_';
					}
				}
				class_name = dup_lcname; /* Syx\\Application => Syx_Application */
			}
		}

		if (strncmp(class_name, SYX_LOADER_RESERVERD, SYX_LOADER_LEN_RESERVERD) == 0) {
			php_error_docref(NULL, E_WARNING, "You should not use '%s' as class name prefix", SYX_LOADER_RESERVERD);
		}

		if(syx_loader_is_app(class_name, class_name_len)){
		    char *pos=NULL, *class_file_name=NULL;
		    pos = strchr(class_name, '_');
		    class_file_name = class_name + (pos + 1 - class_name);
		    file_name_len = strlen(class_file_name);
		    file_name = estrndup(class_file_name, file_name_len);
			spprintf(&directory, 0, "%s", app_directory);
			break;
		}

		file_name_len = class_name_len;
		file_name     = class_name;

	} while(0);

	if (!app_directory && directory) {
		php_error_docref(NULL,
				E_WARNING,
				"Couldn't load a MVC class unless an %s is initialized",
				ZSTR_VAL(syx_application_ce->name));
		ret = 0;
		goto out;
	}

	if (!SYX_G(use_spl_autoload)) {
		/** directory might be NULL since we passed a NULL */
		if (syx_internal_autoload(file_name, file_name_len, &directory)) {
			char *lc_classname = zend_str_tolower_dup(origin_classname, class_name_len);
			if (zend_hash_str_exists(EG(class_table), lc_classname, class_name_len)) {
				efree(lc_classname);
				goto out;
			}
			efree(lc_classname);
			php_error_docref(NULL, E_STRICT, "Syx::Locader Could not find class %s in %s", class_name, directory);
		} else {
			php_error_docref(NULL, E_WARNING, "Failed opening script %s: %s", directory, strerror(errno));
		}
		goto out;
	} else {
		char *lower_case_name = zend_str_tolower_dup(origin_classname, class_name_len);
		if (syx_internal_autoload(file_name, file_name_len, &directory) &&
				zend_hash_str_exists(EG(class_table), lower_case_name, class_name_len)) {
			efree(lower_case_name);
			goto out;
		}
		efree(lower_case_name);
		ret = 0;
		goto out;
	}

out:
	if (directory) {
		efree(directory);
	}
	if (dup_lcname) {
		efree(dup_lcname);
	}
	if (file_name != class_name) {
		efree(file_name);
	}
	RETURN_BOOL(ret);
}
/* }}} */

/** {{{ proto public Syx_Loader::getInstance($library = NULL, $global_library = NULL)
*/
PHP_METHOD(syx_loader, getInstance) {
	zend_string *library = NULL;
	zend_string *global = NULL;
	syx_loader_t *loader, rv = {{0}};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|SS", &library, &global) == FAILURE) {
		return;
	}

	loader = syx_loader_instance(&rv, library, global);
	if (loader) {
		RETURN_ZVAL(loader, 1, 0);
    } else {

	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto private Syx_Loader::__desctruct(void)
*/
PHP_METHOD(syx_loader, __destruct) {
}
/* }}} */

/** {{{ proto syx_override_spl_autoload($class_name)
*/
PHP_FUNCTION(syx_override_spl_autoload) {
	php_error_docref(NULL, E_WARNING, "%s is disabled by ap.use_spl_autoload", SYX_SPL_AUTOLOAD_REGISTER_NAME);
	RETURN_BOOL(0);
}
/* }}} */

/** {{{ syx_loader_methods
*/
zend_function_entry syx_loader_methods[] = {
	PHP_ME(syx_loader, __construct, syx_loader_void_arginfo, ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
	PHP_ME(syx_loader, __clone, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(syx_loader, __sleep, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(syx_loader, __wakeup, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(syx_loader, autoload, syx_loader_autoloader_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_loader, getInstance, syx_loader_getinstance_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(syx_loader, registerLocalNamespace, syx_loader_regnamespace_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_loader, getLocalNamespace, syx_loader_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_loader, clearLocalNamespace, syx_loader_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_loader, isLocalName, syx_loader_islocalname_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_loader, import, syx_loader_import_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(syx_loader, setLibraryPath, syx_loader_setlib_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_loader, getLibraryPath, syx_loader_getlib_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(loader) {
	zend_class_entry ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Loader", syx_loader_methods);
	syx_loader_ce = zend_register_internal_class_ex(&ce, NULL);
	syx_loader_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(syx_loader_ce, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_LIBRARY), 	 ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_loader_ce, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_GLOBAL_LIB), ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_loader_ce, ZEND_STRL(SYX_LOADER_PROPERTY_NAME_INSTANCE),	 ZEND_ACC_PROTECTED|ZEND_ACC_STATIC);

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
