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
#include "main/SAPI.h" /* for sapi_header_line */
#include "Zend/zend_interfaces.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_response.h"
#include "syx_dispatcher.h"
#include "syx_exception.h"

#include "syx_response.h"
#include "responses/syx_response_swoole.h"
#include "responses/syx_response_swoole_tcp.h"
#include "responses/syx_response_swoole_websocket.h"

zend_class_entry *syx_response_swoole_websocket_ce;

/** {{{ ARG_INFO
 */
SYX_BEGIN_ARG_INFO_EX(syx_response_swoole_websocket_void_arginfo, 0, 0, 0)
SYX_END_ARG_INFO()

syx_response_t* syx_response_swoole_websocket_instance(syx_response_t *this_ptr, syx_response_t *swoole_websocket_frame){
    if(Z_ISUNDEF_P(this_ptr)){
        object_init_ex(this_ptr, syx_response_swoole_websocket_ce);
        GET_SOURCE_OBJ_VAR_TO_UPDATE_DEST(swoole_websocket_frame, fd, this_ptr, SYX_RESPONSE_SWOOLE_PROPERTY_NAME_FD);
    }
    return this_ptr;
}

PHP_METHOD(syx_response_swoole_websocket, response){

    syx_response_t *fd, *body, *swoole_server, retval = {{0}}, err_code = {{0}};
    body = zend_read_property(syx_response_swoole_websocket_ce, getThis(), ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), 0, NULL);
    fd = zend_read_property(syx_response_swoole_websocket_ce, getThis(), ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_FD), 0, NULL);
    swoole_server = zend_read_static_property(syx_dispatcher_ce, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_SWOOLE_SERVER), 0);

    if(Z_TYPE_P(fd) == IS_ARRAY){
        zval *value;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(fd), value){
            zend_call_method_with_1_params(swoole_server, Z_OBJCE_P(swoole_server), NULL, "exist", &retval, value);
            if(Z_TYPE_P(&retval) == IS_FALSE){
                zend_call_method_with_0_params(swoole_server, Z_OBJCE_P(swoole_server), NULL, "getLastError", &err_code);
                syx_trigger_error(SYX_ERR_RESPONSE_ERROR, "syx response faild, %l# no exist", Z_LVAL(err_code));
                continue;
            }
            zend_call_method_with_2_params(swoole_server, Z_OBJCE_P(swoole_server), NULL, "push", &retval, value, body);
        }ZEND_HASH_FOREACH_END();
    }else if(Z_TYPE_P(fd) == IS_STRING || Z_TYPE_P(fd) == IS_LONG){
        zend_call_method_with_1_params(swoole_server, Z_OBJCE_P(swoole_server), NULL, "exist", &retval, fd);
    }else{
        syx_trigger_error(SYX_ERR_RESPONSE_ERROR, "syx response faild, fd is null");
        RETURN_FALSE;
    }
    if(Z_TYPE_P(&retval) == IS_FALSE){
        syx_trigger_error(SYX_ERR_RESPONSE_ERROR, "syx response faild, %l# no exist ", Z_LVAL_P(fd));
        RETURN_FALSE;
    }else{
        zend_call_method_with_2_params(swoole_server, Z_OBJCE_P(swoole_server), NULL, "push", &retval, fd, body);
        if(Z_TYPE_P(&retval) == IS_FALSE){
            zend_call_method_with_0_params(swoole_server, Z_OBJCE_P(swoole_server), NULL, "getLastError", &err_code);
            syx_trigger_error(SYX_ERR_RESPONSE_ERROR, "syx response faild, error code:%l", Z_LVAL(err_code));
            RETURN_FALSE;
        }
    }
    RETURN_TRUE;
}

zend_function_entry syx_response_swoole_websocket_methods[] = {
    PHP_ME(syx_response_swoole_websocket, response, syx_response_swoole_websocket_void_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(response_swoole_websocket) {
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\Response\\Swoole\\Websocket", syx_response_swoole_websocket_methods);
    syx_response_swoole_websocket_ce = zend_register_internal_class_ex(&ce, syx_response_swoole_tcp_ce);

    return SUCCESS;
}