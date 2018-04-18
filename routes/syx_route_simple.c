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

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_exception.h"
#include "syx_application.h" /* for syx_application_is_module_name */
#include "syx_request.h"
#include "syx_router.h"

#include "routes/syx_route_interface.h"
#include "routes/syx_route_simple.h"
#include "zend_smart_str.h" /* for smart_str */

zend_class_entry *syx_route_simple_ce;

/** {{{ ARG_INFO
 */
SYX_BEGIN_ARG_INFO_EX(syx_route_simple_construct_arginfo, 0, 0, 3)
	SYX_ARG_INFO(0, module_name)
    SYX_ARG_INFO(0, controller_name)
    SYX_ARG_INFO(0, action_name)
SYX_END_ARG_INFO()
/* }}} */

/** {{{ int syx_route_simple_route(syx_route_t *route, syx_request_t *request)
 */
int syx_route_simple_route(syx_route_t *route, syx_request_t *request) {
	zval *module, *controller, *action;
	zval *nmodule, *ncontroller, *naction;

	nmodule	= zend_read_property(syx_route_simple_ce,
			route, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_MODULE), 1, NULL);
	ncontroller = zend_read_property(syx_route_simple_ce,
			route, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), 1, NULL);
	naction = zend_read_property(syx_route_simple_ce,
			route, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_ACTION), 1, NULL);

	/* if there is no expect parameter in supervars, then null will be return */
	module 	= syx_request_query(SYX_GLOBAL_VARS_GET, Z_STR_P(nmodule));
	controller = syx_request_query(SYX_GLOBAL_VARS_GET, Z_STR_P(ncontroller));
	action = syx_request_query(SYX_GLOBAL_VARS_GET, Z_STR_P(naction));

	if (!module && !controller && !action) {
		return 0;
	}

	if (module && Z_TYPE_P(module) == IS_STRING && syx_application_is_module_name(Z_STR_P(module))) {
		zend_update_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_MODULE), module);
	}

	if (controller) {
		zend_update_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_CONTROLLER), controller);
	}

	if (action) {
		zend_update_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION), action);
	}

	return 1;
}
/* }}} */

/** {{{ syx_route_t * syx_route_simple_instance(syx_route_t *this_ptr, zval *module, zval *controller, zval *action)
 */
syx_route_t * syx_route_simple_instance(syx_route_t *this_ptr, zval *module, zval *controller, zval *action) {
	if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, syx_route_simple_ce);
	}

	zend_update_property(syx_route_simple_ce, this_ptr, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_MODULE), module);
	zend_update_property(syx_route_simple_ce, this_ptr, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), controller);
	zend_update_property(syx_route_simple_ce, this_ptr, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_ACTION), action);

	return this_ptr;
}
/* }}} */

/** {{{ proto public Syx_Route_Simple::route(Syx_Request $req)
*/
PHP_METHOD(syx_route_simple, route) {
	syx_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, syx_request_ce) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(syx_route_simple_route(getThis(), request));
	}
}
/* }}} */

/** {{{ zend_string * syx_route_simple_assemble(zval *info, zval *query)
 */
zend_string * syx_route_simple_assemble(syx_route_t *this_ptr, zval *info, zval *query) {
	smart_str uri = {0};
	zend_string *val;
	zval *nmodule, *ncontroller, *naction;

	smart_str_appendc(&uri, '?');

	nmodule = zend_read_property(syx_route_simple_ce,
			this_ptr, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_MODULE), 1, NULL);
	ncontroller = zend_read_property(syx_route_simple_ce,
			this_ptr, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), 1, NULL);
	naction = zend_read_property(syx_route_simple_ce,
			this_ptr, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_ACTION), 1, NULL);

	do {
		zval *zv;

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(SYX_ROUTE_ASSEMBLE_MOUDLE_FORMAT))) != NULL) {
			val = zval_get_string(zv);
			smart_str_appendl(&uri, Z_STRVAL_P(nmodule), Z_STRLEN_P(nmodule));
			smart_str_appendc(&uri, '=');
			smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
			smart_str_appendc(&uri, '&');
			zend_string_release(val);
		}

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(SYX_ROUTE_ASSEMBLE_CONTROLLER_FORMAT))) == NULL) {
			syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s", "You need to specify the controller by ':c'");
			break;
		}

		val = zval_get_string(zv);
		smart_str_appendl(&uri, Z_STRVAL_P(ncontroller), Z_STRLEN_P(ncontroller));
		smart_str_appendc(&uri, '=');
		smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
		smart_str_appendc(&uri, '&');
		zend_string_release(val);

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(SYX_ROUTE_ASSEMBLE_ACTION_FORMAT))) == NULL) {
			syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s", "You need to specify the action by ':a'");
			break;
		}

		val = zval_get_string(zv);
		smart_str_appendl(&uri, Z_STRVAL_P(naction), Z_STRLEN_P(naction));
		smart_str_appendc(&uri, '=');
		smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
		zend_string_release(val);

		if (query && IS_ARRAY == Z_TYPE_P(query)) {
			zend_string *key;

            ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(query), key, zv) {
				if (key) {
					val = zval_get_string(zv);
					smart_str_appendc(&uri, '&');
					smart_str_appendl(&uri, ZSTR_VAL(key), ZSTR_LEN(key));
					smart_str_appendc(&uri, '=');
					smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
					zend_string_release(val);
				}
			} ZEND_HASH_FOREACH_END();
		}

		smart_str_0(&uri);
		return uri.s;
	} while (0);

	return NULL;
}
/* }}} */

/** {{{ proto public Syx_Route_Simple::__construct(string $module, string $controller, string $action)
 */
PHP_METHOD(syx_route_simple, __construct) {
	zval *module, *controller, *action;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zzz", &module, &controller, &action) == FAILURE) {
		return;
	}

	if (IS_STRING != Z_TYPE_P(module)
			|| IS_STRING != Z_TYPE_P(controller)
			|| IS_STRING != Z_TYPE_P(action)) {
		syx_trigger_error(SYX_ERR_TYPE_ERROR, "Expect 3 string parameters");
		RETURN_FALSE;
	} else {
	    zval rself, *self = getThis();

	    if (!self) {
	        ZVAL_NULL(&rself);
	        self = &rself;
        }
		(void)syx_route_simple_instance(self, module, controller, action);
	}
}
/* }}} */

/** {{{ proto public Syx_Route_Simple::assemble(array $info[, array $query = NULL])
 */
PHP_METHOD(syx_route_simple, assemble) {
    zval *info, *query = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
        return;
    } else {
		zend_string *str;
        if ((str = syx_route_simple_assemble(getThis(), info, query)) != NULL) {
			RETURN_STR(str);
		}
		RETURN_NULL();
    }
}
/* }}} */

/** {{{ syx_route_simple_methods
 */
zend_function_entry syx_route_simple_methods[] = {
	PHP_ME(syx_route_simple, __construct, syx_route_simple_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(syx_route_simple, route, syx_route_route_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_route_simple, assemble, syx_route_assemble_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
 */
SYX_STARTUP_FUNCTION(route_simple) {
	zend_class_entry ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Route\\Simple", syx_route_simple_methods);
	syx_route_simple_ce = zend_register_internal_class(&ce);
	zend_class_implements(syx_route_simple_ce, 1, syx_route_ce);

	syx_route_simple_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(syx_route_simple_ce, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_CONTROLLER), ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_route_simple_ce, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_MODULE), ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_route_simple_ce, ZEND_STRL(SYX_ROUTE_SIMPLE_VAR_NAME_ACTION), ZEND_ACC_PROTECTED);

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
