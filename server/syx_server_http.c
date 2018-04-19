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
#include "syx_dispatcher.h"
#include "syx_bootstrap.h"
#include "syx_exception.h"

#include "syx_server.h"
#include "server/syx_server_interface.h"
#include "server/syx_server_plugin.h"
#include "server/syx_server_tcp.h"
#include "server/syx_server_http.h"
#include "server/syx_server_websocket.h"
#include "requests/syx_request_swoole_http.h"
#include "responses/syx_response_swoole_http.h"

zend_class_entry *syx_server_http_ce;

PHP_METHOD(syx_server_http, start){
    syx_server_t *syx_swoole_server_o, *self;

    self = getThis();
    syx_swoole_server_o = zend_read_static_property(syx_dispatcher_ce, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_SWOOLE_SERVER), 0);
    if (UNEXPECTED(syx_swoole_server_o == NULL)) {
        RETURN_FALSE;
    }
    syx_server_swoole_server_construct(syx_swoole_server_o, self);
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_HTTP_EVENT_REQUEST), ZEND_STRL(SYX_SERVER_HTTP_EVENT_REQUEST_CALLBACK));
    syx_server_start(self, syx_swoole_server_o);
    RETURN_TRUE;
}

PHP_METHOD(syx_server_http, onRequest){
    syx_request_t syx_request={{0}}, *swoole_http_request = NULL;
    syx_response_t *swoole_http_response = NULL, syx_response = {{0}};
    syx_dispatcher_t *syx_dispatcher = NULL;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "oo", &swoole_http_request, &swoole_http_response) == FAILURE) {
        return;
    }
    SYX_G(sapi) = SYX_SERVER_TYPE_SWOOLE_HTTP;
    syx_dispatcher = syx_dispatcher_instance(NULL);
    (void)syx_request_swoole_http_instance(&syx_request, swoole_http_request);
    (void)syx_dispatcher_set_request(syx_dispatcher, &syx_request);
    (void)syx_response_swoole_http_instance(&syx_response, swoole_http_response);
    (void)syx_dispatcher_dispatch(syx_dispatcher, &syx_response);
    syx_server_destruct(syx_dispatcher, &syx_request, &syx_response);
}

/** {{{ syx_server_abstract_methods
 */
zend_function_entry syx_server_http_methods[] = {
    PHP_ME(syx_server_http, start, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_http, onRequest, syx_server_interface_request_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
 */
SYX_STARTUP_FUNCTION(server_http) {
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\Server\\Http", syx_server_http_methods);
    syx_server_http_ce = zend_register_internal_class_ex(&ce, syx_server_tcp_ce);
    zend_declare_property_string(syx_server_http_ce, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_SERVER_CLASS), SYX_SERVER_CLASS_SWOOLE_SERVER_HTTP, ZEND_ACC_PROTECTED);

    return SUCCESS;
}