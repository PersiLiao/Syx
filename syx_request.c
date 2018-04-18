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
#include "standard/php_string.h" /* for php_basename */
#include "Zend/zend_exceptions.h" /* for zend_exception_get_default */

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_request.h"
#include "syx_exception.h"

#include "requests/syx_request_simple.h"
#include "requests/syx_request_http.h"
#include "requests/syx_request_swoole_tcp.h"
#include "requests/syx_request_swoole_http.h"
//#include "requests/syx_request_swoole_udp.h"
#include "requests/syx_request_swoole_websocket.h"

zend_class_entry *syx_request_ce;

/** {{{ ARG_INFO
*/
SYX_BEGIN_ARG_INFO_EX(syx_request_set_routed_arginfo, 0, 0, 0)
	SYX_ARG_INFO(0, flag)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_request_set_module_name_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, module)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_request_set_controller_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, controller)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_request_set_action_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, action)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_request_set_baseuri_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, uri)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_request_set_request_uri_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, uri)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_request_set_param_arginfo, 0, 0, 2)
	SYX_ARG_INFO(0, name)
	SYX_ARG_INFO(0, value)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_request_get_param_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, name)
	SYX_ARG_INFO(0, default)
SYX_END_ARG_INFO()
	/* }}} */

void syx_request_params_init(syx_zval_t* this_ptr){
    zval params = {{0}};
    array_init(&params);
    zend_update_property(syx_request_http_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), &params);
    zval_ptr_dtor(&params);
}

int syx_request_is_routed(syx_request_t *request) /* {{{ */{
	zval *routed = zend_read_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ROUTED), 1, NULL);
	return Z_TYPE_P(routed) == IS_TRUE ? 1 : 0;
}
/* }}} */

int syx_request_is_dispatched(syx_request_t *request) /* {{{ */ {
	zval *dispatched = zend_read_property(syx_request_ce,
			request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_STATE), 1, NULL);
	return Z_TYPE_P(dispatched) == IS_TRUE ? 1 : 0;
}
/* }}} */

void syx_request_set_dispatched(syx_request_t *instance, int flag) /* {{{ */ {
	zend_update_property_bool(syx_request_ce, instance, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_STATE), flag);
}
/* }}} */

void syx_request_set_routed(syx_request_t *request, int flag) /* {{{ */ {
	zend_update_property_bool(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ROUTED), flag);
}
/* }}} */

int syx_request_set_params_single(syx_request_t *request, zend_string *key, zval *value) /* {{{ */ {
	zval *params = zend_read_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
	if ((zend_hash_update(Z_ARRVAL_P(params), key, value)) != NULL) {
		Z_TRY_ADDREF_P(value);
		return 1;
	}
	return 0;
}
/* }}} */

int syx_request_set_params_multi(syx_request_t *request, zval *values) /* {{{ */ {
	zval *params = zend_read_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
	if (values && Z_TYPE_P(values) == IS_ARRAY) {
		zend_hash_copy(Z_ARRVAL_P(params), Z_ARRVAL_P(values), (copy_ctor_func_t) zval_add_ref);
		return 1;
	}
	return 0;
}
/* }}} */

zval * syx_request_get_param(syx_request_t *request, zend_string *key) /* {{{ */ {
	zval *params = zend_read_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
	return zend_hash_find(Z_ARRVAL_P(params), key);
}
/* }}} */


/** {{{ proto public Syx_Request_Abstract::getModuleName(void)
*/

PHP_METHOD(syx_request, getModuleName) {
	zval *module = zend_read_property(syx_request_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_MODULE), 1, NULL);
	RETVAL_ZVAL(module, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::getControllerName(void)
*/
PHP_METHOD(syx_request, getControllerName) {
	zval *controller = zend_read_property(syx_request_ce,
		   	getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_CONTROLLER), 1, NULL);
	RETVAL_ZVAL(controller, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::getActionName(void)
*/
PHP_METHOD(syx_request, getActionName) {
	zval *action = zend_read_property(syx_request_ce,
			getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION), 1, NULL);
	RETVAL_ZVAL(action, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::setModuleName(string $module)
*/
PHP_METHOD(syx_request, setModuleName) {
	zval *module;
	syx_request_t *self	= getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &module) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(module) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "Expect a string module name");
		RETURN_FALSE;
	}

	zend_update_property(syx_request_ce, self, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_MODULE), module);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::setControllerName(string $controller)
*/
PHP_METHOD(syx_request, setControllerName) {
	zval *controller;
	syx_request_t *self	= getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &controller) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(controller) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "Expect a string controller name");
		RETURN_FALSE;
	}

	zend_update_property(syx_request_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_CONTROLLER), controller);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::setActionName(string $action)
*/
PHP_METHOD(syx_request, setActionName) {
	zval *action;
	zval *self	 = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &action) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(action) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "Expect a string action name");
		RETURN_FALSE;
	}

	zend_update_property(syx_request_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION), action);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::setParam(mixed $value)
*/
PHP_METHOD(syx_request, setParam) {
	uint argc;
	syx_request_t *self	= getThis();

	argc = ZEND_NUM_ARGS();

	if (1 == argc) {
		zval *value ;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &value) == FAILURE) {
			return;
		}
		if (syx_request_set_params_multi(self, value)) {
			RETURN_ZVAL(self, 1, 0);
		}
	} else if (2 == argc) {
		zval *value;
		zend_string *name;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
			return;
		}

		if (syx_request_set_params_single(getThis(), name, value)) {
			RETURN_ZVAL(self, 1, 0);
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::getParam(string $name, $mixed $default = NULL)
*/
PHP_METHOD(syx_request, getParam) {
	zend_string *name;
	zval *def = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def) == FAILURE) {
		return;
	} else {
		zval *value = syx_request_get_param(getThis(), name);
		if (value) {
			RETURN_ZVAL(value, 1, 0);
		}
		if (def) {
			RETURN_ZVAL(def, 1, 0);
		}
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::getException(void)
*/
PHP_METHOD(syx_request, getException) {
	zval *exception = zend_read_property(syx_request_ce,
			getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_EXCEPTION), 1, NULL);
	if (IS_OBJECT == Z_TYPE_P(exception)
			&& instanceof_function(Z_OBJCE_P(exception), zend_exception_get_default())) {
		RETURN_ZVAL(exception, 1, 0);
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::getParams(void)
*/
PHP_METHOD(syx_request, getParams) {
	zval *params = zend_read_property(syx_request_ce,
			getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
	RETURN_ZVAL(params, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::getMethod(void)
*/
PHP_METHOD(syx_request, getMethod) {
	zval *method = syx_request_get_method(getThis());
	RETURN_ZVAL(method, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::isDispatched(void)
*/
PHP_METHOD(syx_request, isDispatched) {
	RETURN_BOOL(syx_request_is_dispatched(getThis()));
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::setDispatched(void)
*/
PHP_METHOD(syx_request, setDispatched) {
	syx_request_set_dispatched(getThis(), 1);
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::setBaseUri(string $name)
*/
PHP_METHOD(syx_request, setBaseUri) {
	zend_string *uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &uri) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(uri) == 0) {
		RETURN_FALSE;
	}

	if (syx_request_set_base_uri(getThis(), uri, NULL)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::getBaseUri(string $name)
*/
PHP_METHOD(syx_request, getBaseUri) {
	zval *uri = zend_read_property(syx_request_ce,
			getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_BASE), 1, NULL);
	RETURN_ZVAL(uri, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::getRequestUri(string $name)
*/
PHP_METHOD(syx_request, getRequestUri) {
	zval *uri = zend_read_property(syx_request_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_URI), 1, NULL);
	RETURN_ZVAL(uri, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::setRequestUri(string $name)
*/
PHP_METHOD(syx_request, setRequestUri) {
	zend_string *uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &uri) == FAILURE) {
		return;
	}

	zend_update_property_str(syx_request_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_URI), uri);
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::isRouted(void)
*/
PHP_METHOD(syx_request, isRouted) {
	RETURN_BOOL(syx_request_is_routed(getThis()));
}
/* }}} */

/** {{{ proto public Syx_Request_Abstract::setRouted(void)
*/
PHP_METHOD(syx_request, setRouted) {
	syx_request_set_routed(getThis(), 1);
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto private Syx_Request_Abstract::__clone
 */
PHP_METHOD(syx_request, __clone) {
}
/* }}} */

/** {{{ syx_request_methods
*/
zend_function_entry syx_request_methods[] = {
    PHP_ABSTRACT_ME(syx_request, getRaw, syx_request_void_arginfo)
	PHP_ME(syx_request, setParam, syx_request_set_param_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, getParam, syx_request_get_param_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, getParams, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, getException, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, getModuleName, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, getControllerName, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, getActionName, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, setModuleName, syx_request_set_module_name_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, setControllerName, syx_request_set_controller_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, setActionName, syx_request_set_action_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, getMethod, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, setBaseUri, syx_request_set_baseuri_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, getBaseUri, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, getRequestUri, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, setRequestUri, syx_request_set_request_uri_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, isDispatched, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, setDispatched, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, isRouted, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request, setRouted, syx_request_set_routed_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(request){
	zend_class_entry ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\RequestAbstract", syx_request_methods);
	syx_request_ce 			= zend_register_internal_class_ex(&ce, NULL);
	syx_request_ce->ce_flags = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_MODULE), ZEND_ACC_PUBLIC);
	zend_declare_property_null(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_CONTROLLER), ZEND_ACC_PUBLIC);
	zend_declare_property_null(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION),     ZEND_ACC_PUBLIC);
	zend_declare_property_null(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_METHOD), 	ZEND_ACC_PUBLIC);
	zend_declare_property_null(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS),  	ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_LANG), 		ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_EXCEPTION),  ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PROTOCOL),  ZEND_ACC_PROTECTED);

	zend_declare_property_string(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_BASE), "", ZEND_ACC_PROTECTED);
	zend_declare_property_string(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_URI),  "", ZEND_ACC_PROTECTED);
	zend_declare_property_bool(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_STATE),	0,	ZEND_ACC_PROTECTED);
	zend_declare_property_bool(syx_request_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ROUTED), 0, 	ZEND_ACC_PROTECTED);

	SYX_STARTUP(request_http);
	SYX_STARTUP(request_simple);
	SYX_STARTUP(request_swoole_tcp);
	SYX_STARTUP(request_swoole_http);
	SYX_STARTUP(request_swoole_websocket);

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
