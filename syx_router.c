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
#include "Zend/zend_interfaces.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_exception.h"
#include "syx_application.h" /* for syx_application_is_module_name */
#include "syx_request.h" /* for syx_request_set_routed */
#include "syx_router.h"
#include "syx_config.h"

#include "routes/syx_route_interface.h"
#include "routes/syx_route_static.h"
#include "routes/syx_route_simple.h"
#include "routes/syx_route_supervar.h"
#include "routes/syx_route_regex.h"
#include "routes/syx_route_rewrite.h"
#include "routes/syx_route_restful.h"
#include "routes/syx_route_map.h"

zend_class_entry *syx_router_ce;

/** {{{ syx_router_t * syx_router_instance(syx_router_t *this_ptr)
 */
syx_router_t * syx_router_instance(syx_router_t *this_ptr) {
	zval routes;
	syx_route_t	route = {{0}};

    if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, syx_router_ce);
	}

	array_init(&routes);
	if (!SYX_G(default_route)) {
static_route:
		object_init_ex(&route, syx_route_static_ce);
	} else {
		(void)syx_route_instance(&route, SYX_G(default_route));
		if (Z_TYPE(route) != IS_OBJECT) {
			php_error_docref(NULL, E_WARNING,
					"Unable to initialize default route, use %s instead", ZSTR_VAL(syx_route_static_ce->name));
			goto static_route;
		}
	}

	zend_hash_str_update(Z_ARRVAL(routes), "_default", sizeof("_default") - 1, &route);
	zend_update_property(syx_router_ce, this_ptr, ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_ROUTES), &routes);
	zval_ptr_dtor(&routes);

	return this_ptr;
}
/** }}} */

/** {{{ int syx_router_route(syx_router_t *router, syx_request_t *request)
*/
int syx_router_route(syx_router_t *router, syx_request_t *request) {
	zval *routers, ret;
	syx_route_t	*route;
	HashTable 	*ht;
	zend_string *key;
	zend_long idx;

	routers = zend_read_property(syx_router_ce, router, ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_ROUTES), 1, NULL);

	ht = Z_ARRVAL_P(routers);

	ZEND_HASH_REVERSE_FOREACH_KEY_VAL(ht, idx, key, route) {
		zend_call_method_with_1_params(route, Z_OBJCE_P(route), NULL, "route", &ret, request);
		if (IS_TRUE == Z_TYPE(ret)) {
			if (key) {
				zend_update_property_string(syx_router_ce,
						router, ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_CURRENT_ROUTE), ZSTR_VAL(key));
			} else {
				zend_update_property_long(syx_router_ce,
						router, ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_CURRENT_ROUTE), idx);
			}
			syx_request_set_routed(request, 1);
			return 1;
		} else {
			zval_ptr_dtor(&ret);
			continue;
		}
	} ZEND_HASH_FOREACH_END();

	return 0;
}
/* }}} */

/** {{{ int syx_router_add_config(syx_router_t *router, zval *configs)
*/
int syx_router_add_config(syx_router_t *router, zval *configs) {
	zval 		*entry;
	syx_route_t *route, rv;

	if (!configs || IS_ARRAY != Z_TYPE_P(configs)) {
		return 0;
	} else {
		ulong idx;
		zend_string *key;
		zval *routes;

		routes = zend_read_property(syx_router_ce, router, ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_ROUTES), 1, NULL);

		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(configs), idx, key, entry) {
			if (Z_TYPE_P(entry) != IS_ARRAY) {
				continue;
			}

			ZVAL_UNDEF(&rv);
			route = syx_route_instance(&rv, entry);
			if (key) {
				if (!route) {
					php_error_docref(NULL, E_WARNING, "Unable to initialize route named '%s'", ZSTR_VAL(key));
					continue;
				}
				zend_hash_update(Z_ARRVAL_P(routes), key, route);
			} else {
				if (!route) {
					php_error_docref(NULL, E_WARNING, "Unable to initialize route at index '%ld'", idx);
					continue;
				}
				zend_hash_index_update(Z_ARRVAL_P(routes), idx, route);
			}
		} ZEND_HASH_FOREACH_END();
		return 1;
	}
}
/* }}} */

/** {{{ void syx_router_parse_parameters(char *uri, zval *params)
 */
void syx_router_parse_parameters(char *uri, zval *params) {
	char *key, *ptrptr, *tmp, *value;
	zval val;
	uint key_len;

	array_init(params);

	tmp = estrdup(uri);
	key = php_strtok_r(tmp, SYX_ROUTER_URL_DELIMIETER, &ptrptr);
	while (key) {
		key_len = strlen(key);
		if (key_len) {
			value = php_strtok_r(NULL, SYX_ROUTER_URL_DELIMIETER, &ptrptr);
			if (value && strlen(value)) {
				ZVAL_STRING(&val, value);
			} else {
				ZVAL_NULL(&val);
			}
			zend_hash_str_update(Z_ARRVAL_P(params), key, key_len, &val);
		}

		key = php_strtok_r(NULL, SYX_ROUTER_URL_DELIMIETER, &ptrptr);
	}

	efree(tmp);
}
/* }}} */

/** {{{ proto public Syx_Router::__construct(void)
 */
PHP_METHOD(syx_router, __construct) {
	syx_router_instance(getThis());
}
/* }}} */

/** {{{ proto public Syx_Router::route(Syx_Request $req)
*/
PHP_METHOD(syx_router, route) {
	syx_request_t *request;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &request) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(syx_router_route(getThis(), request));
	}
}
/* }}} */

/** {{{  proto public Syx_Router::addRoute(string $name, Syx_Route_Interface $route)
 */
PHP_METHOD(syx_router, addRoute) {
	zend_string 	   *name = NULL;
	zval 	   *routes;
	syx_route_t *route;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &route) == FAILURE) {
		return;
	}

	if (!name) {
		RETURN_FALSE;
	}

	if (IS_OBJECT != Z_TYPE_P(route)
			|| !instanceof_function(Z_OBJCE_P(route), syx_route_ce)) {
		php_error_docref(NULL, E_WARNING, "Expects a %s instance", ZSTR_VAL(syx_route_ce->name));
		RETURN_FALSE;
	}

	routes = zend_read_property(syx_router_ce, getThis(), ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_ROUTES), 1, NULL);

	Z_TRY_ADDREF_P(route);
	zend_hash_update(Z_ARRVAL_P(routes), name, route);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{  proto public Syx_Router::addConfig(Syx_Config_Abstract $config)
 */
PHP_METHOD(syx_router, addConfig) {
	syx_config_t *config;
	zval *routes;
	zval *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &config) == FAILURE) {
		return;
	}

	if (IS_OBJECT == Z_TYPE_P(config) && instanceof_function(Z_OBJCE_P(config), syx_config_ce)){
		routes = zend_read_property(syx_config_ce, config, ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
	} else if (IS_ARRAY == Z_TYPE_P(config)) {
		routes = config;
	} else {
		php_error_docref(NULL, E_WARNING,
				"Expect a %s instance or an array, %s given",
				ZSTR_VAL(syx_config_ce->name), zend_zval_type_name(config));
		RETURN_FALSE;
	}

	if (syx_router_add_config(self, routes)) {
		RETURN_ZVAL(self, 1, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{  proto public Syx_Router::getRoute(string $name)
 */
PHP_METHOD(syx_router, getRoute) {
	zend_string  *name;
	zval  *routes;
	syx_route_t *route;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(name) == 0) {
		RETURN_FALSE;
	}

	routes = zend_read_property(syx_router_ce, getThis(), ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_ROUTES), 1, NULL);

	if ((route = zend_hash_find(Z_ARRVAL_P(routes), name)) != NULL) {
		RETURN_ZVAL(route, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{  proto public Syx_Router::getRoutes(void)
 */
PHP_METHOD(syx_router, getRoutes) {
	zval *routes = zend_read_property(syx_router_ce,
			getThis(), ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_ROUTES), 1, NULL);
	RETURN_ZVAL(routes, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Router::isModuleName(string $name)
 */
PHP_METHOD(syx_router, isModuleName) {
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	RETURN_BOOL(syx_application_is_module_name(name));
}
/* }}} */

/** {{{  proto public Syx_Router::getCurrentRoute(void)
 */
PHP_METHOD(syx_router, getCurrentRoute) {
	zval *route = zend_read_property(syx_router_ce,
			getThis(), ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_CURRENT_ROUTE), 1, NULL);
	RETURN_ZVAL(route, 1, 0);
}
/* }}} */

/** {{{ syx_router_methods
 */
zend_function_entry syx_router_methods[] = {
	PHP_ME(syx_router, __construct, 	NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(syx_router, addRoute,  		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_router, addConfig, 		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_router, route,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_router, getRoute,  		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_router, getRoutes, 		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_router, getCurrentRoute, 	NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
 */
SYX_STARTUP_FUNCTION(router) {
	zend_class_entry ce;
	(void)syx_route_route_arginfo; /* tricky, supress warning "defined but not used" */

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Router", syx_router_methods);
	syx_router_ce = zend_register_internal_class_ex(&ce, NULL);

	syx_router_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(syx_router_ce, ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_ROUTES), 		 ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_router_ce, ZEND_STRL(SYX_ROUTER_PROPERTY_NAME_CURRENT_ROUTE), ZEND_ACC_PROTECTED);

	SYX_STARTUP(route);
	SYX_STARTUP(route_static);
	SYX_STARTUP(route_simple);
	SYX_STARTUP(route_supervar);
	SYX_STARTUP(route_rewrite);
	SYX_STARTUP(route_restful);
	SYX_STARTUP(route_regex);
	SYX_STARTUP(route_map);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
