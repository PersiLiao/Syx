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
#include "syx_dispatcher.h"
#include "syx_response.h"
#include "syx_exception.h"

#include "responses/syx_response_swoole.h"
#include "responses/syx_response_swoole_tcp.h"

zend_class_entry *syx_response_swoole_tcp_ce;

/** {{{ ARG_INFO
 */
SYX_BEGIN_ARG_INFO_EX(syx_response_swoole_tcp_construct_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, fd)
SYX_END_ARG_INFO()

syx_response_t* syx_response_swoole_tcp_instance(syx_response_t *this_ptr, syx_response_t *fd){

    if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, syx_response_swoole_tcp_ce);
    }
    zend_update_property(syx_response_swoole_tcp_ce, this_ptr, ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_FD), fd);
    return this_ptr;
}

/** {{{ proto public Syx_Response_Abstract::response(void)
 */
PHP_METHOD(syx_response_swoole_tcp, __construct) {
    syx_response_t *fd;
    if(zend_parse_parameters_throw(ZEND_NUM_ARGS(), "l", &fd) == FAILURE){
        return;
    }
    zend_update_property(syx_response_swoole_tcp_ce, getThis(), ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_FD), fd);
}
/* }}} */

/** {{{ proto public Syx_Response_Abstract::response(void)
 */
PHP_METHOD(syx_response_swoole_tcp, response) {
    syx_response_t *fd, *body, *swoole_server, retval = {{0}}, err_code = {{0}};

    body = zend_read_property(syx_response_swoole_tcp_ce, getThis(), ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), 0, NULL);
    fd = zend_read_property(syx_response_swoole_tcp_ce, getThis(), ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_FD), 0, NULL);
    swoole_server = zend_read_static_property(syx_dispatcher_ce, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_SWOOLE_SERVER), 0);
    zend_call_method_with_2_params(swoole_server, Z_OBJCE_P(swoole_server), NULL, "send", &retval, fd, body);
    if(Z_TYPE_P(&retval) == IS_FALSE){
        zend_call_method_with_0_params(swoole_server, Z_OBJCE_P(swoole_server), NULL, "getLastError", &err_code);
        syx_trigger_error(SYX_ERR_RESPONSE_ERROR, "syx response faild, error code:%l", Z_LVAL(err_code));
        RETURN_FALSE;
    }
    RETURN_TRUE;
}
/* }}} */

zend_function_entry syx_response_swoole_tcp_methods[] = {
    PHP_ME(syx_response_swoole_tcp, __construct, syx_response_swoole_tcp_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(syx_response_swoole_tcp, response, syx_response_void_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(response_swoole_tcp) {
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\Response\\Swoole\\Tcp", syx_response_swoole_tcp_methods);
    syx_response_swoole_tcp_ce = zend_register_internal_class_ex(&ce, syx_response_swoole_ce);

    return SUCCESS;
}