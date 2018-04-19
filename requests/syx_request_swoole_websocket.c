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
#include "syx_config.h"
#include "syx_loader.h"
#include "syx_application.h"
#include "syx_bootstrap.h"
#include "syx_dispatcher.h"
#include "syx_exception.h"

#include "syx_request.h"
#include "requests/syx_request_http.h"
#include "requests/syx_request_swoole_tcp.h"
#include "requests/syx_request_swoole_http.h"
#include "requests/syx_request_swoole_websocket.h"

zend_class_entry* syx_request_swoole_websocket_ce;

SYX_BEGIN_ARG_INFO_EX(syx_request_swoole_websocket_construct_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(1, swoole_websocket_frame, Swoole\\Websocket\\Frame, 0)
SYX_END_ARG_INFO()

syx_request_t *syx_request_swoole_websocket_instance(syx_request_t *this_ptr, syx_request_t *swoole_websocket_frame){
    zend_string *settled_uri = NULL;
    syx_request_t *request_http_server;

    if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, syx_request_swoole_websocket_ce);
    }

    GET_SOURCE_OBJ_VAR_TO_UPDATE_DEST(swoole_websocket_frame, fd, this_ptr, SYX_REQUEST_PROPERTY_NAME_FD);
    GET_SOURCE_OBJ_VAR_TO_UPDATE_DEST(swoole_websocket_frame, opcode, this_ptr, SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_OPCODE);
    GET_SOURCE_OBJ_VAR_TO_UPDATE_DEST(swoole_websocket_frame, data, this_ptr, SYX_REQUEST_PROPERTY_NAME_DATA);
    GET_SOURCE_OBJ_VAR_TO_UPDATE_DEST(swoole_websocket_frame, finish, this_ptr, SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_FINISH);
    zend_update_property(syx_request_swoole_websocket_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_RAW), swoole_websocket_frame);
    zend_update_property_string(syx_request_swoole_websocket_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_METHOD), "WEBSOCKET");
    settled_uri = zend_string_init(ZEND_STRL("/"), 0);
    syx_request_set_base_uri(this_ptr, SYX_G(base_uri), settled_uri);
    zend_string_release(settled_uri);
    syx_request_params_init(this_ptr);

    return this_ptr;
}

PHP_METHOD(syx_request_swoole_websocket, getOpcode){
    syx_request_t *ret;
    ret = zend_read_property(syx_request_swoole_websocket_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_OPCODE), 0, NULL);
    RETURN_ZVAL(ret, 0, 0);
    RETURN_TRUE;
}

PHP_METHOD(syx_request_swoole_websocket, getFinish){
    syx_request_t *ret;
    ret = zend_read_property(syx_request_swoole_websocket_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_FINISH), 0, NULL);
    RETURN_ZVAL(ret, 0, 0);
}

PHP_METHOD(syx_request_swoole_websocket, getData){
    syx_request_t *ret;
    ret = zend_read_property(syx_request_swoole_websocket_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_DATA), 0, NULL);
    RETURN_ZVAL(ret, 0, 0);
}

PHP_METHOD(syx_request_swoole_websocket, getRaw){
    syx_request_t *ret;
    ret = zend_read_property(syx_request_swoole_websocket_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_RAW), 0, NULL);
    RETURN_ZVAL(ret, 0, 1);
}

PHP_METHOD(syx_request_swoole_websocket, __construct){
    syx_request_t *swoole_websocket_frame;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "O", &swoole_websocket_frame) == FAILURE) {
        RETURN_FALSE;
    }
    (void)syx_request_swoole_websocket_instance(getThis(), swoole_websocket_frame);
    RETURN_TRUE;
}

PHP_METHOD(syx_request_swoole_websocket, isTextData){
    syx_request_t *ret;
    ret = zend_read_property(syx_request_swoole_websocket_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_OPCODE), 0, NULL);
    if(Z_LVAL_P(ret) == SYX_WEBSOCKET_OPCODE_TEXT_FRAME){
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

PHP_METHOD(syx_request_swoole_websocket, isBinaryData){
    syx_request_t *ret;
    ret = zend_read_property(syx_request_swoole_websocket_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_OPCODE), 0, NULL);
    if(Z_LVAL_P(ret) == SYX_WEBSOCKET_OPCODE_BINARY_FRAME){
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

zend_function_entry syx_request_swoole_websocket_methods[] = {

    PHP_ME(syx_request_swoole_websocket, __construct, syx_request_swoole_websocket_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(syx_request_swoole_websocket, getRaw, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_websocket, getData, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_websocket, getOpcode, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_websocket, getFinish, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_websocket, isTextData, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_websocket, isBinaryData, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(request_swoole_websocket){
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\Request\\Swoole\\Websocket", syx_request_swoole_websocket_methods);
    syx_request_swoole_websocket_ce = zend_register_internal_class_ex(&ce, syx_request_swoole_tcp_ce);
    zend_declare_property_null(syx_request_swoole_websocket_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_OPCODE), ZEND_ACC_PROTECTED);
    zend_declare_property_null(syx_request_swoole_websocket_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_FINISH), ZEND_ACC_PROTECTED);
    zend_declare_property_null(syx_request_swoole_websocket_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_DATA), ZEND_ACC_PROTECTED);

    return SUCCESS;
}