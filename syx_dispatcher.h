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

#ifndef PHP_SYX_DISPATCHER_H
#define PHP_SYX_DISPATCHER_H

#define SYX_DISPATCHER_PROPERTY_NAME_REQUEST    "_request"
#define SYX_DISPATCHER_PROPERTY_NAME_VIEW       "_view"
#define SYX_DISPATCHER_PROPERTY_NAME_CONTORLLER_DIR       "_controller_dir"
#define SYX_DISPATCHER_PROPERTY_NAME_ROUTER     "_router"
#define SYX_DISPATCHER_PROPERTY_NAME_INSTANCE   "_instance"
#define SYX_DISPATCHER_PROPERTY_NAME_RENDER     "_auto_render"
#define SYX_DISPATCHER_PROPERTY_NAME_RETURN     "_return_response"
#define SYX_DISPATCHER_PROPERTY_NAME_FLUSH      "_instantly_flush"
#define SYX_DISPATCHER_PROPERTY_NAME_ARGS       "_invoke_args"

#define SYX_DISPATCHER_PROPERTY_NAME_MODULE     "_default_module"
#define SYX_DISPATCHER_PROPERTY_NAME_CONTROLLER "_default_controller"
#define SYX_DISPATCHER_PROPERTY_NAME_ACTION     "_default_action"

#define SYX_ERROR_CONTROLLER                    "Error"
#define SYX_ERROR_ACTION                        "error"

#define SYX_DISPATCHER_PROPERTY_NAME_PLUGINS    "_plugins"

#define SYX_PLUGIN_HOOK_ROUTESTARTUP            "routerstartup"
#define SYX_PLUGIN_HOOK_ROUTESHUTDOWN           "routershutdown"
#define SYX_PLUGIN_HOOK_LOOPSTARTUP             "dispatchloopstartup"
#define SYX_PLUGIN_HOOK_PREDISPATCH             "predispatch"
#define SYX_PLUGIN_HOOK_POSTDISPATCH            "postdispatch"
#define SYX_PLUGIN_HOOK_LOOPSHUTDOWN            "dispatchloopshutdown"
#define SYX_PLUGIN_HOOK_PRERESPONSE             "preresponse"

#define SYX_PLUGIN_HANDLE(p, n, request, response) \
	do { \
		if (!ZVAL_IS_NULL(p)) { \
			zval *_t_plugin;\
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(p), _t_plugin) { \
			    if (zend_hash_str_exists(&(Z_OBJCE_P(_t_plugin)->function_table), n, sizeof(n) - 1)) { \
			        zend_call_method_with_2_params(_t_plugin, Z_OBJCE_P(_t_plugin), NULL, n, NULL, request, response); \
				} \
			} ZEND_HASH_FOREACH_END(); \
		} \
	} while(0)

extern zend_class_entry *syx_dispatcher_ce;

syx_dispatcher_t *syx_dispatcher_instance(syx_dispatcher_t *this_ptr);
syx_response_t *syx_dispatcher_dispatch(syx_dispatcher_t *dispatcher, zval *response_ptr);
int syx_dispatcher_set_request(syx_dispatcher_t *dispatcher, syx_request_t *request);

PHP_METHOD(syx_application, app);
PHP_FUNCTION(set_error_handler);
SYX_STARTUP_FUNCTION(dispatcher);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
