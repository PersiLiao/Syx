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
#include "syx_server.h"
#include "requests/syx_request_swoole_tcp.h"

zend_class_entry *syx_request_swoole_tcp_ce;

SYX_BEGIN_ARG_INFO_EX(syx_request_swoole_tcp_construct_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, data)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_request_swoole_tcp_data_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, data)
SYX_END_ARG_INFO()

syx_request_t *syx_request_swoole_tcp_instance(syx_request_t *this_ptr, syx_request_t *fd, syx_request_t *reactor_id, syx_request_t *data){
    zval params, *request_uri, *request_method, *instance;
    zend_string *settled_uri = NULL;
    HashTable *request_http;

    if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, syx_request_swoole_tcp_ce);
    }

    zend_update_property_string(syx_request_swoole_tcp_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_METHOD), "TCP");
    zend_update_property(syx_request_swoole_tcp_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_FD), fd );
    zend_update_property(syx_request_swoole_tcp_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_REACTOR_ID), reactor_id );
    zend_update_property(syx_request_swoole_tcp_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_DATA), data );
    zend_update_property(syx_request_swoole_tcp_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_RAW), data );
    settled_uri = zend_string_init(ZEND_STRL("/"), 0);
    syx_request_set_base_uri(this_ptr, SYX_G(base_uri), settled_uri);
    zend_string_release(settled_uri);
    syx_request_params_init(this_ptr);

    return this_ptr;
}

PHP_METHOD(syx_request_swoole_tcp, __construct){
    syx_request_t *fd, *reactor_id, *data;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "llz", &fd, &reactor_id, &data) == FAILURE) {
        RETURN_FALSE;
    }
    (void)syx_request_swoole_tcp_instance(getThis(), fd, reactor_id, data);
    RETURN_TRUE;
}

PHP_METHOD(syx_request_swoole_tcp, getFd){
    syx_request_t *ret;
    ret = zend_read_property(syx_request_swoole_tcp_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_FD), 0, NULL);
    RETURN_ZVAL(ret, 0, 0);
}

PHP_METHOD(syx_request_swoole_tcp, getReactorId){
    syx_request_t *ret;
    ret = zend_read_property(syx_request_swoole_tcp_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_REACTOR_ID), 0, NULL);
    RETURN_ZVAL(ret, 0, 0);
}

PHP_METHOD(syx_request_swoole_tcp, getData){
    syx_request_t *ret;
    ret = zend_read_property(syx_request_swoole_tcp_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_DATA), 0, NULL);
    RETURN_ZVAL(ret, 0, 0);
}

PHP_METHOD(syx_request_swoole_tcp, setData){
    syx_request_t *data = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &data) == FAILURE) {
        RETURN_FALSE;
    }
    zend_update_property(syx_request_swoole_tcp_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_DATA), data);
    RETURN_TRUE;
}

PHP_METHOD(syx_request_swoole_tcp, getRaw){
    syx_request_t *ret;
    ret = zend_read_property(syx_request_swoole_tcp_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_RAW), 0, NULL);
    RETURN_ZVAL(ret, 0, 0);
}

zend_function_entry syx_request_swoole_tcp_methods[] = {
    PHP_ME(syx_request_swoole_tcp, getRaw, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_tcp, getFd, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_tcp, getReactorId, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_tcp, getData, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_tcp, setData, syx_request_swoole_tcp_data_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_tcp, __construct, syx_request_swoole_tcp_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(request_swoole_tcp){
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\Request\\Swoole\\Tcp", syx_request_swoole_tcp_methods);
    syx_request_swoole_tcp_ce = zend_register_internal_class_ex(&ce, syx_request_ce);

    zend_declare_property_null(syx_request_swoole_tcp_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_RAW), ZEND_ACC_PROTECTED);
    zend_declare_property_null(syx_request_swoole_tcp_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_FD), ZEND_ACC_PROTECTED);
    zend_declare_property_null(syx_request_swoole_tcp_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_REACTOR_ID), ZEND_ACC_PROTECTED);
    zend_declare_property_null(syx_request_swoole_tcp_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_DATA), ZEND_ACC_PROTECTED);

    return SUCCESS;
}