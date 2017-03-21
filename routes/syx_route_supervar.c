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
#include "syx_request.h"

#include "syx_router.h"
#include "routes/syx_route_interface.h"
#include "routes/syx_route_static.h" /* for syx_route_pathinfo_route */
#include "routes/syx_route_supervar.h"
#include "zend_smart_str.h" /* for smart_str */

zend_class_entry *syx_route_supervar_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(syx_route_supervar_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, supervar_name)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ int syx_route_supervar_route(syx_route_t *route, syx_request_t *request)
 */
int syx_route_supervar_route(syx_route_t *route, syx_request_t *request) {
	zval *varname, *uri;
	zend_string *req_uri;

	varname = zend_read_property(syx_route_supervar_ce,
			route, ZEND_STRL(SYX_ROUTE_SUPERVAR_PROPETY_NAME_VAR), 1, NULL);

	uri = syx_request_query(SYX_GLOBAL_VARS_GET, Z_STR_P(varname));

	if (!uri) {
		return 0;
	}

	req_uri = zend_string_init(Z_STRVAL_P(uri), Z_STRLEN_P(uri), 0);
	syx_route_pathinfo_route(request, req_uri);
	zend_string_release(req_uri);
	return 1;
}
/* }}} */

syx_route_t * syx_route_supervar_instance(syx_route_t *this_ptr, zval *name) /* {{{ */ {
	if (!name || IS_STRING != Z_TYPE_P(name) || !Z_STRLEN_P(name)) {
		return NULL;
	}

    if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, syx_route_supervar_ce);
	}

	zend_update_property(syx_route_supervar_ce, this_ptr, ZEND_STRL(SYX_ROUTE_SUPERVAR_PROPETY_NAME_VAR), name);

	return this_ptr;
}
/* }}} */

/** {{{ proto public Syx_Route_Supervar::route(Syx_Request_Abstarct $request)
 */
PHP_METHOD(syx_route_supervar, route) {
	syx_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, syx_request_ce) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(syx_route_supervar_route(getThis(), request));
	}
}
/** }}} */

/** {{{ zend_string * syx_route_supervar_assemble(zval *info, zval *query)
 */
zend_string * syx_route_supervar_assemble(syx_route_t *this_ptr, zval *info, zval *query) {
	smart_str uri = {0};
	zend_string *val;
	zval *pname, *zv;

	pname = zend_read_property(syx_route_supervar_ce,
			this_ptr, ZEND_STRL(SYX_ROUTE_SUPERVAR_PROPETY_NAME_VAR), 1, NULL);

	do {
		smart_str_appendc(&uri, '?');
		smart_str_appendl(&uri, Z_STRVAL_P(pname), Z_STRLEN_P(pname));
		smart_str_appendc(&uri, '=');

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(SYX_ROUTE_ASSEMBLE_MOUDLE_FORMAT))) != NULL) {
			val = zval_get_string(zv);
			smart_str_appendc(&uri, '/');
			smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
			zend_string_release(val);
		}

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(SYX_ROUTE_ASSEMBLE_CONTROLLER_FORMAT))) == NULL) {
			syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s", "You need to specify the controller by ':c'");
			break;
		}

		val = zval_get_string(zv);
		smart_str_appendc(&uri, '/');
		smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
		zend_string_release(val);

		if ((zv = zend_hash_str_find(Z_ARRVAL_P(info), ZEND_STRL(SYX_ROUTE_ASSEMBLE_ACTION_FORMAT))) == NULL) {
			syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s", "You need to specify the action by ':a'");
			break;
		}

		val = zval_get_string(zv);
		smart_str_appendc(&uri, '/');
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

	smart_str_free(&uri);
	return NULL;
}
/* }}} */

/** {{{ proto public Syx_Route_Supervar::__construct(string $varname)
*/
PHP_METHOD(syx_route_supervar, __construct) {
    zval *var;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &var) ==   FAILURE) {
        return;
    }

    if (Z_TYPE_P(var) != IS_STRING || !Z_STRLEN_P(var)) {
        syx_trigger_error(SYX_ERR_TYPE_ERROR, "Expects a valid string super var name");
        RETURN_FALSE;
    }

    zend_update_property(syx_route_supervar_ce, getThis(), ZEND_STRL(SYX_ROUTE_SUPERVAR_PROPETY_NAME_VAR), var);
}
/** }}} */

/** {{{ proto public Syx_Route_Supervar::assemble(array $info[, array $query = NULL])
*/
PHP_METHOD(syx_route_supervar, assemble) {
    zval *info, *query = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
        return;
    } else {
		zend_string *str;
        if ((str = syx_route_supervar_assemble(getThis(), info, query)) != NULL) {
			RETURN_STR(str);
		}
		RETURN_NULL();
    }

}
/* }}} */

/** {{{ syx_route_supervar_methods
*/
zend_function_entry syx_route_supervar_methods[] = {
	PHP_ME(syx_route_supervar, __construct, syx_route_supervar_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(syx_route_supervar, route, syx_route_route_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_route_supervar, assemble, syx_route_assemble_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
 */
SYX_STARTUP_FUNCTION(route_supervar) {
	zend_class_entry ce;
	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Route\\Supervar", syx_route_supervar_methods);
	syx_route_supervar_ce = zend_register_internal_class(&ce);
	zend_class_implements(syx_route_supervar_ce, 1, syx_route_ce);
	syx_route_supervar_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(syx_route_supervar_ce, ZEND_STRL(SYX_ROUTE_SUPERVAR_PROPETY_NAME_VAR),  ZEND_ACC_PROTECTED);

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

