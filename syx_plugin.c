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
#include "syx_plugin.h"

zend_class_entry * syx_plugin_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(plugin_arg_ns, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, request, Syx\\RequestAbstract, 0)
	ZEND_ARG_OBJ_INFO(0, response, Syx\\ResponseAbstract, 0)
ZEND_END_ARG_INFO()
/* }}} */

/** {{{ proto public Syx_Plugin::routerStartup(Syx_Request_Abstract $request, Syx_Response_Abstract $response)
*/
PHP_METHOD(syx_plugin, routerStartup) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Plugin::routerShutdown(Syx_Request_Abstract $request, Syx_Response_Abstract $response)
*/
PHP_METHOD(syx_plugin, routerShutdown) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Plugin::dispatchLoopStartup(Syx_Request_Abstract $request, Syx_Response_Abstract $response)
*/
PHP_METHOD(syx_plugin, dispatchLoopStartup) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Plugin::preDispatch(Syx_Request_Abstract $request, Syx_Response_Abstract $response)
*/
PHP_METHOD(syx_plugin, preDispatch) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Plugin::postDispatch(Syx_Request_Abstract $request, Syx_Response_Abstract $response)
*/
PHP_METHOD(syx_plugin, postDispatch) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Plugin::dispatchLoopShutdown(Syx_Request_Abstract $request, Syx_Response_Abstract $response)
*/
PHP_METHOD(syx_plugin, dispatchLoopShutdown) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Plugin::preResponse(Syx_Request_Abstract $request, Syx_Response_Abstract $response)
*/
PHP_METHOD(syx_plugin, preResponse) {
	RETURN_TRUE;
}
/* }}} */

/** {{{ syx_plugin_methods
*/
zend_function_entry syx_plugin_methods_ns[] = {
	PHP_ME(syx_plugin, routerStartup,  		 plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(syx_plugin, routerShutdown,  		 plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(syx_plugin, dispatchLoopStartup,   plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(syx_plugin, dispatchLoopShutdown,  plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(syx_plugin, preDispatch,  		 plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(syx_plugin, postDispatch, 		 plugin_arg_ns, ZEND_ACC_PUBLIC)
	PHP_ME(syx_plugin, preResponse, 			 plugin_arg_ns, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(plugin) {
	zend_class_entry ce;
	SYX_INIT_CLASS_ENTRY(ce, "Syx\\PluginAbstract", syx_plugin_methods_ns);
	syx_plugin_ce = zend_register_internal_class_ex(&ce, NULL);
	syx_plugin_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

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
