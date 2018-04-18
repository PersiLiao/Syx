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
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_request.h"
#include "syx_response.h"
#include "syx_dispatcher.h"
#include "syx_view.h"
#include "syx_exception.h"
#include "syx_action.h"
#include "syx_controller.h"

zend_class_entry * syx_controller_ce;

/** {{{ ARG_INFO
 */
SYX_BEGIN_ARG_INFO_EX(syx_controller_void_arginfo, 0, 0, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_controller_initview_arginfo, 0, 0, 0)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_controller_getiarg_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, name)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_controller_setvdir_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, view_directory)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_controller_forward_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, module)
    SYX_ARG_INFO(0, controller)
    SYX_ARG_INFO(0, action)
    ZEND_ARG_ARRAY_INFO(0, parameters, 1)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_controller_redirect_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, url)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_controller_render_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, tpl)
    ZEND_ARG_ARRAY_INFO(0, parameters, 1)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_controller_display_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, tpl)
    ZEND_ARG_ARRAY_INFO(0, parameters, 1)
SYX_END_ARG_INFO()
/* }}} */

zend_string * syx_controller_render(syx_controller_t *instance, char *action_name, int len, zval *var_array) /* {{{ */ {
	char *self_name, *tmp;
	zval *name, param, ret;
	syx_view_t *view;
	zend_class_entry *view_ce;
	zend_string *path, *view_ext;

	view = zend_read_property(syx_controller_ce,
			instance, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_VIEW), 1, NULL);
	name = zend_read_property(syx_controller_ce,
			instance, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_NAME), 1, NULL);
	view_ext = SYX_G(view_ext);

	self_name = zend_str_tolower_dup(Z_STRVAL_P(name), Z_STRLEN_P(name));

	tmp = self_name;
 	while (*tmp != '\0') {
		if (*tmp == '_') {
			*tmp = DEFAULT_SLASH;
		}
		tmp++;
	}

	action_name = estrndup(action_name, len);

	tmp = action_name;
 	while (*tmp != '\0') {
		if (*tmp == '_') {
			*tmp = DEFAULT_SLASH;
		}
		tmp++;
	}

	path = strpprintf(0, "%s%c%s.%s", self_name, DEFAULT_SLASH, action_name, ZSTR_VAL(view_ext));

	efree(self_name);
	efree(action_name);

	ZVAL_STR(&param, path);

	view_ce = Z_OBJCE_P(view);
	if (var_array) {
		zend_call_method_with_2_params(view, view_ce, NULL, "render", &ret, &param, var_array);
	} else {
		zend_call_method_with_1_params(view, view_ce, NULL, "render", &ret, &param);
	}

	zval_ptr_dtor(&param);

	if (Z_ISUNDEF(ret)) {
		return NULL;
	}

	if (EG(exception)) {
		zval_ptr_dtor(&ret);
		return NULL;
	}

	if (Z_TYPE(ret) != IS_STRING) {
		zval_ptr_dtor(&ret);
		return NULL;
	}

	return Z_STR(ret);
}
/* }}} */

/** {{{ int syx_controller_display(syx_controller_t *instance, char *action_name, int len, zval *var_array)
 */
int syx_controller_display(syx_controller_t *instance, char *action_name, int len, zval *var_array) {
	char *self_name, *tmp;
	zval *name, param, ret;
	syx_view_t	*view;
	zend_string *path, *view_ext;

	view = zend_read_property(syx_controller_ce, instance, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_VIEW), 1, NULL);
	name = zend_read_property(syx_controller_ce, instance, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_NAME), 1, NULL);
	view_ext  = SYX_G(view_ext);

	self_name = zend_str_tolower_dup(Z_STRVAL_P(name), Z_STRLEN_P(name));

	tmp = self_name;
	while (*tmp != '\0') {
		if (*tmp == '_') {
			*tmp = DEFAULT_SLASH;
		}
		tmp++;
	}

	action_name = estrndup(action_name, len);

	tmp = action_name;
 	while (*tmp != '\0') {
		if (*tmp == '_') {
			*tmp = DEFAULT_SLASH;
		}
		tmp++;
	}

	path = strpprintf(0, "%s%c%s.%s", self_name, DEFAULT_SLASH, action_name, ZSTR_VAL(view_ext));

	efree(self_name);
	efree(action_name);

	ZVAL_STR(&param, path);

	if (var_array) {
		zend_call_method_with_2_params(view, Z_OBJCE_P(view), NULL, "display", &ret, &param, var_array);
	} else {
		zend_call_method_with_1_params(view, Z_OBJCE_P(view), NULL, "display", &ret, &param);
	}
	zval_ptr_dtor(&param);

	if (Z_ISUNDEF(ret)) {
		return 0;
	}

	if (EG(exception)) {
		zval_ptr_dtor(&ret);
		return 0;
	}

	if (Z_TYPE(ret) == IS_FALSE) {
		return 0;
	}

	zval_ptr_dtor(&ret);

	return 1;
}
/* }}} */

/** {{{ int syx_controller_construct(zend_class_entry *ce, syx_controller_t *self, syx_request_t *request, syx_response_t *responseew_t *view, zval *args)
 */
int syx_controller_construct(zend_class_entry *ce, syx_controller_t *self, syx_request_t *request, syx_response_t *response, syx_view_t *view, zval *args) {
	zval *module;

	if (args) {
		zend_update_property(ce, self, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_ARGS), args);
	}

	module = zend_read_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_MODULE), 1, NULL);

	zend_update_property(ce, self, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_REQUEST), request);
	zend_update_property(ce, self, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_RESPONSE), response);
	zend_update_property(ce, self, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_MODULE), module);
	zend_update_property(ce, self, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_VIEW), view);

	if (!instanceof_function(ce, syx_action_ce)
			&& zend_hash_str_exists(&(ce->function_table), ZEND_STRL("init"))) {
		zend_call_method_with_0_params(self, ce, NULL, "init", NULL);
	}

	return 1;
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::init()
*/
PHP_METHOD(syx_controller, init) {
}

/* }}} */

/** {{{ proto protected Syx_Controller_Abstract::__construct(Syx_Request_Abstract $request, Syx_Response_abstrct $response, Syx_View_Interface $view, array $invokeArgs = NULL)
*/
PHP_METHOD(syx_controller, __construct) {
	syx_request_t 	*request;
	syx_response_t	*response;
	syx_view_t		*view;
	zval 			*invoke_arg = NULL;
	syx_controller_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OOO|z",
		&request, syx_request_ce, &response, syx_response_ce, &view, syx_view_interface_ce, &invoke_arg) == FAILURE) {
		return;
	}
	if (!syx_controller_construct(syx_controller_ce, self, request, response, view, invoke_arg)) {
		RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::getView(void)
*/
PHP_METHOD(syx_controller, getView) {
	syx_view_t *view = zend_read_property(syx_controller_ce,
			getThis(), ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_VIEW), 1, NULL);
	RETURN_ZVAL(view, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::getRequest(void)
*/
PHP_METHOD(syx_controller, getRequest) {
	syx_request_t *request = zend_read_property(syx_controller_ce,
			getThis(), ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_REQUEST), 1, NULL);
	RETURN_ZVAL(request, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::getResponse(void)
*/
PHP_METHOD(syx_controller, getResponse) {
	syx_response_t *response = zend_read_property(syx_controller_ce,
			getThis(), ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_RESPONSE), 1, NULL);
	RETURN_ZVAL(response, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::initView(array $options = NULL)
*/
PHP_METHOD(syx_controller, initView) {
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::getInvokeArg(string $name)
 */
PHP_METHOD(syx_controller, getInvokeArg) {
	zend_string *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S",  &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (name) {
		zval *pzval, *args;
		args = zend_read_property(syx_controller_ce, getThis(), ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_ARGS), 1, NULL);

		if (ZVAL_IS_NULL(args)) {
			RETURN_NULL();
		}

		if ((pzval = zend_hash_find(Z_ARRVAL_P(args), name)) != NULL) {
			RETURN_ZVAL(pzval, 1, 0);
		}
	}
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::getInvokeArgs(void)
 */
PHP_METHOD(syx_controller, getInvokeArgs) {
	zval *args = zend_read_property(syx_controller_ce,
			getThis(), ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_ARGS), 1, NULL);
	RETURN_ZVAL(args, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::getModuleName(void)
 */
PHP_METHOD(syx_controller, getModuleName) {
	zval *module = zend_read_property(syx_controller_ce,
			getThis(), ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_MODULE), 1, NULL);
	RETURN_ZVAL(module, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::setViewpath(string $view_directory)
*/
PHP_METHOD(syx_controller, setViewpath) {
	zval 		 *path;
	syx_view_t 	 *view;
	zend_class_entry *view_ce;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &path) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(path) != IS_STRING) {
		RETURN_FALSE;
	}

	view = zend_read_property(syx_controller_ce, getThis(), ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_VIEW), 1, NULL);
	if (EXPECTED((view_ce = Z_OBJCE_P(view)) == syx_view_simple_ce)) {
		zend_update_property(view_ce, view, ZEND_STRL(SYX_VIEW_PROPERTY_NAME_TPLDIR), path);
	} else {
		zend_call_method_with_1_params(view, view_ce, NULL, "setscriptpath", NULL, path);
	}

	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::getViewpath(void)
*/
PHP_METHOD(syx_controller, getViewpath) {
	zend_class_entry *view_ce;
	zval *view = zend_read_property(syx_controller_ce,
			getThis(), ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_VIEW), 1, NULL);
	if (EXPECTED((view_ce = Z_OBJCE_P(view)) == syx_view_simple_ce)) {
		zval *tpl_dir = zend_read_property(view_ce, view, ZEND_STRL(SYX_VIEW_PROPERTY_NAME_TPLDIR), 1, NULL);
		if (IS_STRING != Z_TYPE_P(tpl_dir) && SYX_G(view_directory)) {
			RETURN_STR(zend_string_copy(SYX_G(view_directory)));
		}
		RETURN_ZVAL(tpl_dir, 1, 0);
	} else {
		zval ret;
		zend_call_method_with_0_params(view, view_ce, NULL, "getscriptpath", &ret);
		if (Z_ISUNDEF(ret)) {
			RETURN_NULL();
		}
		RETURN_ZVAL(&ret, 0, 0);
	}
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::forward($module, $controller, $action, $args = NULL)
*/
PHP_METHOD(syx_controller, forward) {
	syx_request_t *request;
	zval *controller, *module, *action, *args, *parameters;
	zend_class_entry *request_ce;

	syx_controller_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|zzz", &module, &controller, &action, &args) == FAILURE) {
		return;
	}

	request = zend_read_property(syx_controller_ce, self, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_REQUEST), 1, NULL);

	if (UNEXPECTED(Z_TYPE_P(request) != IS_OBJECT ||
		!instanceof_function((request_ce = Z_OBJCE_P(request)), syx_request_ce))) {
		RETURN_FALSE;
	}

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (Z_TYPE_P(module) != IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Expect a string action name");
				RETURN_FALSE;
			}
			zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION), module);
			break;
		case 2:
			if (Z_TYPE_P(controller) ==  IS_STRING) {
				zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_CONTROLLER), module);
				zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION), controller);
			} else if (Z_TYPE_P(controller) == IS_ARRAY) {
				parameters = zend_read_property(request_ce,
						request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
				zend_hash_copy(Z_ARRVAL_P(parameters), Z_ARRVAL_P(controller), (copy_ctor_func_t) zval_add_ref);
				zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION), module);
				zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), parameters);
			} else {
				RETURN_FALSE;
			}
			break;
		case 3:
			if (Z_TYPE_P(action) == IS_STRING) {
				zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_MODULE), module);
				zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_CONTROLLER), controller);
				zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION), action);
			} else if (Z_TYPE_P(action) == IS_ARRAY) {
				parameters = zend_read_property(request_ce,
						request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
				zend_hash_copy(Z_ARRVAL_P(parameters), Z_ARRVAL_P(action), (copy_ctor_func_t) zval_add_ref);
				zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_CONTROLLER), module);
				zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION), controller);
				zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), parameters);
			} else {
				RETURN_FALSE;
			}
			break;
		case 4:
			if (Z_TYPE_P(args) != IS_ARRAY) {
				php_error_docref(NULL, E_WARNING, "Parameters must be an array");
				RETURN_FALSE;
			}
			parameters = zend_read_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), 1, NULL);
			zend_hash_copy(Z_ARRVAL_P(parameters), Z_ARRVAL_P(args), (copy_ctor_func_t) zval_add_ref);
			zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_MODULE), module);
			zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_CONTROLLER), controller);
			zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_ACTION), action);
			zend_update_property(request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), parameters);
			break;
	}

	(void)syx_request_set_dispatched(request, 0);
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Controller_Abstract::redirect(string $url)
*/
PHP_METHOD(syx_controller, redirect) {
	char 			*location;
	size_t 			location_len;
	syx_response_t 		*response;
	syx_controller_t 	*self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &location, &location_len) == FAILURE) {
		return;
	}

	response = zend_read_property(syx_controller_ce, self, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_RESPONSE), 1, NULL);

	(void)syx_response_set_redirect(response, location, location_len);

	RETURN_TRUE;
}
/* }}} */

/** {{{ proto protected Syx_Controller_Abstract::render(string $action, array $var_array = NULL)
*/
PHP_METHOD(syx_controller, render) {
	char *action_name;
	size_t action_name_len;
	zval *var_array	= NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|z", &action_name, &action_name_len, &var_array) == FAILURE) {
		return;
	} else {
		zend_string *output = syx_controller_render(getThis(), action_name, action_name_len, var_array);
		if (output) {
			RETURN_STR(output);
		} else {
			RETURN_FALSE;
		}
	}
}
/* }}} */

/** {{{ proto protected Syx_Controller_Abstract::display(string $action, array $var_array = NULL)
*/
PHP_METHOD(syx_controller, display) {
	char *action_name;
	size_t action_name_len;
	zval *var_array	= NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|z", &action_name, &action_name_len, &var_array) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(syx_controller_display(getThis(), action_name, action_name_len, var_array));
	}
}
/* }}} */

/** {{{ proto private Syx_Controller_Abstract::__clone()
*/
PHP_METHOD(syx_controller, __clone) {
}
/* }}} */

/** {{{ syx_controller_methods
*/
zend_function_entry syx_controller_methods[] = {
	PHP_ME(syx_controller, render,	    syx_controller_render_arginfo, 	ZEND_ACC_PROTECTED)
	PHP_ME(syx_controller, display,	    syx_controller_display_arginfo, ZEND_ACC_PROTECTED)
	PHP_ME(syx_controller, getRequest,	syx_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, getResponse,	syx_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, getModuleName,syx_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, getView,		syx_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, initView,	syx_controller_initview_arginfo,ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, setViewpath,	syx_controller_setvdir_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, getViewpath,	syx_controller_void_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, forward,	   	syx_controller_forward_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, redirect,    syx_controller_redirect_arginfo,ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, getInvokeArgs,syx_controller_void_arginfo,   ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, getInvokeArg, syx_controller_getiarg_arginfo,ZEND_ACC_PUBLIC)
	PHP_ME(syx_controller, __construct,	NULL, 							ZEND_ACC_PUBLIC|ZEND_ACC_CTOR|ZEND_ACC_FINAL)
	PHP_ME(syx_controller, __clone, 	NULL, 							ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(controller) {
	zend_class_entry ce;
	SYX_INIT_CLASS_ENTRY(ce, "Syx\\ControllerAbstract", syx_controller_methods);
	syx_controller_ce = zend_register_internal_class_ex(&ce, NULL);
	syx_controller_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(syx_controller_ce, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_ACTIONS),	ZEND_ACC_PUBLIC);
	zend_declare_property_null(syx_controller_ce, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_MODULE), 	ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_controller_ce, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_NAME), 	ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_controller_ce, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_REQUEST),	ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_controller_ce, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_RESPONSE),	ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_controller_ce, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_ARGS),		ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_controller_ce, ZEND_STRL(SYX_CONTROLLER_PROPERTY_NAME_VIEW),		ZEND_ACC_PROTECTED);

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
