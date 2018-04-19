/*
  +----------------------------------------------------------------------+
  | Syx Framework                                                        |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: XiangChu Liao  <ican.lxc@gmail.com>                          |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "Zend/zend_interfaces.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_plugin.h"
#include "syx_server_plugin.h"

zend_class_entry * syx_server_plugin_ce;

/** {{{ ARG_INFO
 */
SYX_BEGIN_ARG_INFO_EX(server_plugin_arg_ns, 0, 0, 2)
    ZEND_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    ZEND_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()
/* }}} */

PHP_METHOD(syx_server_plugin, beforeStart){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeShutdown){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeManageStart){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeManageStop){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeWorkerStart){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterWorkerStart){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeTaskStart){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterTaskStart){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeWorkerError){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeWorkerExit){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeWorkerStop){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeConnect){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterConnect){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeReceive){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterReceive){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeRequest){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterRequest){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforePacket){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterPacket){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeClose){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeTask){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeTaskFinish){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterTaskFinish){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforePipeMessage){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterPipeMessage){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeHandshake){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterHandshake){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeOpen){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterOpen){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeMessage){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, afterMessage){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeBufferFull){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_plugin, beforeBufferEmpty){
    RETURN_TRUE;
}


zend_function_entry syx_server_plugin_methods_ns[] = {
    PHP_ME(syx_server_plugin, beforeStart, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeShutdown, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeManageStart, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeManageStop, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeWorkerStart, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterWorkerStart, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeTaskStart, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterTaskStart, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeWorkerError, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeWorkerExit, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeWorkerStop, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeConnect, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterConnect, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeReceive, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterReceive, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeRequest, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterRequest, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforePacket, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterPacket, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeClose, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeTask, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeTaskFinish, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterTaskFinish, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforePipeMessage, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterPipeMessage, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeHandshake, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterHandshake, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeOpen, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterOpen, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeMessage, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, afterMessage, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeBufferFull, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_plugin, beforeBufferEmpty, server_plugin_arg_ns, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(server_plugin) {
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\Server\\PluginAbstract", syx_server_plugin_methods_ns);
    syx_server_plugin_ce = zend_register_internal_class_ex(&ce, syx_plugin_ce);
    syx_server_plugin_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

    return SUCCESS;
}