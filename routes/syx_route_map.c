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
#include "syx_exception.h"
#include "syx_request.h"

#include "syx_router.h"
#include "routes/syx_route_interface.h"
#include "routes/syx_route_map.h"

zend_class_entry *syx_route_map_ce;

/** {{{ ARG_INFO
 */
SYX_BEGIN_ARG_INFO_EX(syx_route_map_construct_arginfo, 0, 0, 0)
    SYX_ARG_INFO(0, controller_prefer)
	SYX_ARG_INFO(0, delimiter)
SYX_END_ARG_INFO()
/* }}} */

syx_route_t * syx_route_map_instance(syx_route_t *this_ptr, zend_bool controller_prefer, zend_string *delim) /* {{{ */{
	if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, syx_route_map_ce);
	}

	if (controller_prefer) {
		zend_update_property_bool(syx_route_map_ce, this_ptr,
				ZEND_STRL(SYX_ROUTE_MAP_VAR_NAME_CTL_PREFER), 1);
	}

	if (delim && ZSTR_LEN(delim)) {
		zend_update_property_str(syx_route_map_ce, this_ptr, ZEND_STRL(SYX_ROUTE_MAP_VAR_NAME_DELIMETER), delim);
	}

	return this_ptr;
}
/* }}} */

/** {{{ int syx_route_map_route(syx_route_t *route, syx_request_t *request)
*/
int syx_route_map_route(syx_route_t *route, syx_request_t *request) {
	zval *ctl_prefer, *delimer, *zuri, *base_uri, params;
	char *req_uri, *tmp, *rest, *ptrptr, *seg;
	char *query_str = NULL;
	uint seg_len = 0;

	smart_str route_result = {0};

	zuri = zend_read_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_URI), 1, NULL);
	base_uri = zend_read_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_BASE), 1, NULL);

	ctl_prefer = zend_read_property(syx_route_map_ce, route, ZEND_STRL(SYX_ROUTE_MAP_VAR_NAME_CTL_PREFER), 1, NULL);
	delimer	   = zend_read_property(syx_route_map_ce, route, ZEND_STRL(SYX_ROUTE_MAP_VAR_NAME_DELIMETER), 1, NULL);

	if (base_uri && IS_STRING == Z_TYPE_P(base_uri)
			&& !strncasecmp(Z_STRVAL_P(zuri), Z_STRVAL_P(base_uri), Z_STRLEN_P(base_uri))) {
		req_uri  = estrdup(Z_STRVAL_P(zuri) + Z_STRLEN_P(base_uri));
	} else {
		req_uri  = estrdup(Z_STRVAL_P(zuri));
	}

	if (Z_TYPE_P(delimer) == IS_STRING
			&& Z_STRLEN_P(delimer)) {
		if ((query_str = strstr(req_uri, Z_STRVAL_P(delimer))) != NULL
			&& *(query_str - 1) == '/') {
			tmp  = req_uri;
			rest = query_str + Z_STRLEN_P(delimer);
			if (*rest == '\0') {
				req_uri 	= estrndup(req_uri, query_str - req_uri);
				query_str 	= NULL;
				efree(tmp);
			} else if (*rest == '/') {
				req_uri 	= estrndup(req_uri, query_str - req_uri);
				query_str   = estrdup(rest);
				efree(tmp);
			} else {
				query_str = NULL;
			}
		} else {
			query_str = NULL;
		}
	}

	seg = php_strtok_r(req_uri, SYX_ROUTER_URL_DELIMIETER, &ptrptr);
	while (seg) {
		seg_len = strlen(seg);
		if (seg_len) {
			smart_str_appendl(&route_result, seg, seg_len);
		}
		smart_str_appendc(&route_result, '_');
		seg = php_strtok_r(NULL, SYX_ROUTER_URL_DELIMIETER, &ptrptr);
	}

	if (route_result.s) {
		ZSTR_LEN(route_result.s)--;
		ZSTR_VAL(route_result.s)[ZSTR_LEN(route_result.s)] = '\0';
		if (Z_TYPE_P(ctl_prefer) == IS_TRUE) {
			zend_update_property_str(syx_request_ce,
					request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_CONTROLLER), route_result.s);
		} else {
			zend_update_property_str(syx_request_ce,
					request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION), route_result.s);
		}
		smart_str_free(&route_result);
	}

	if (query_str) {
		(void)syx_router_parse_parameters(query_str, &params);
		(void)syx_request_set_params_multi(request, &params);
		zval_ptr_dtor(&params);
		efree(query_str);
	}

	efree(req_uri);

	return 1;
}
/* }}} */

/** {{{ proto public Syx_Route_Simple::route(Syx_Request $req)
*/
PHP_METHOD(syx_route_map, route) {
	syx_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &request, syx_request_ce) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(syx_route_map_route(getThis(), request));
	}
}
/* }}} */

/** {{{ zend_string * syx_route_map_assemble(zval *info, zval *query)
 */
zend_string * syx_route_map_assemble(syx_route_t *this_ptr, zval *info, zval *query) {
	char *seg, *ptrptr, *pname;
	smart_str uri = {0};
	size_t seg_len;
	zend_bool has_delim = 0;
	zval *delim, *ctl_prefer, *zv;

	ctl_prefer = zend_read_property(syx_route_map_ce, this_ptr, ZEND_STRL(SYX_ROUTE_MAP_VAR_NAME_CTL_PREFER), 1, NULL);
	delim = zend_read_property(syx_route_map_ce, this_ptr, ZEND_STRL(SYX_ROUTE_MAP_VAR_NAME_DELIMETER), 1, NULL);

	if (IS_STRING == Z_TYPE_P(delim) && Z_STRLEN_P(delim)) {
		has_delim = 1;
	}

	do {
		if (Z_TYPE_P(ctl_prefer) == IS_TRUE) {
			if ((zv = zend_hash_str_find(Z_ARRVAL_P(info),
					ZEND_STRL(SYX_ROUTE_ASSEMBLE_ACTION_FORMAT))) != NULL && Z_TYPE_P(zv) == IS_STRING) {
				pname = estrndup(Z_STRVAL_P(zv), Z_STRLEN_P(zv));
			} else {
				syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s",
						"Undefined the 'action' parameter for the 1st parameter");
				break;
			}
		} else {
			if ((zv = zend_hash_str_find(Z_ARRVAL_P(info),
					ZEND_STRL(SYX_ROUTE_ASSEMBLE_CONTROLLER_FORMAT))) != NULL && Z_TYPE_P(zv) == IS_STRING) {
				pname = estrndup(Z_STRVAL_P(zv), Z_STRLEN_P(zv));
			} else {
				syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s",
						"Undefined the 'controller' parameter for the 1st parameter");
				break;
			}
		}

		seg = php_strtok_r(pname, "_", &ptrptr);
		while (seg) {
			seg_len = strlen(seg);
			if (seg_len) {
				smart_str_appendc(&uri, '/');
				smart_str_appendl(&uri, seg, seg_len);
			}
			seg = php_strtok_r(NULL, "_", &ptrptr);
		}
		efree(pname);

		if (query && IS_ARRAY == Z_TYPE_P(query)) {
			zend_bool start = 1;
			zend_string *key, *val;

			if (has_delim) {
				smart_str_appendc(&uri, '/');
				smart_str_appendl(&uri, Z_STRVAL_P(delim), Z_STRLEN_P(delim));
			}

            ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(query), key, zv) {
				if (key) {
					val = zval_get_string(zv);
					if (has_delim) {
						smart_str_appendc(&uri, '/');
						smart_str_appendl(&uri, ZSTR_VAL(key), ZSTR_LEN(key));
						smart_str_appendc(&uri, '/');
						smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
					} else {
						if (start) {
							smart_str_appendc(&uri, '?');
							smart_str_appendl(&uri, ZSTR_VAL(key), ZSTR_LEN(key));
							smart_str_appendc(&uri, '=');
							smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
							start = 0;
						} else {
							smart_str_appendc(&uri, '&');
							smart_str_appendl(&uri, ZSTR_VAL(key), ZSTR_LEN(key));
							smart_str_appendc(&uri, '=');
							smart_str_appendl(&uri, ZSTR_VAL(val), ZSTR_LEN(val));
						}
					}
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

/** {{{ proto public Syx_Route_Simple::__construct(bool $controller_prefer=FALSE, string $delimer = '#!')
*/
PHP_METHOD(syx_route_map, __construct) {
	zend_string *delim	= NULL;
	zend_bool controller_prefer = 0;
	zval *self = getThis();

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|bS", &controller_prefer, &delim) == FAILURE) {
		return;
	}

	(void)syx_route_map_instance(self, controller_prefer, delim);
}
/* }}} */

/** {{{ proto public Syx_Route_Map::assemble(array $info[, array $query = NULL])
*/
PHP_METHOD(syx_route_map, assemble) {
	zval *info, *query = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|a", &info, &query) == FAILURE) {
        return;
    } else {
		zend_string *str;
        if ((str = syx_route_map_assemble(getThis(), info, query)) != NULL) {
			RETURN_STR(str);
		}
		RETURN_NULL();
    }
}
/* }}} */

/** {{{ syx_route_map_methods
*/
zend_function_entry syx_route_map_methods[] = {
	PHP_ME(syx_route_map, __construct, syx_route_map_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(syx_route_map, route, syx_route_route_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_route_map, assemble, syx_route_assemble_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(route_map) {
	zend_class_entry ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Route\\Map", syx_route_map_methods);
	syx_route_map_ce = zend_register_internal_class(&ce);
	zend_class_implements(syx_route_map_ce, 1, syx_route_ce);

	syx_route_map_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_bool(syx_route_map_ce, ZEND_STRL(SYX_ROUTE_MAP_VAR_NAME_CTL_PREFER), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_route_map_ce, ZEND_STRL(SYX_ROUTE_MAP_VAR_NAME_DELIMETER),  ZEND_ACC_PROTECTED);

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
