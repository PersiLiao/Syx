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

zend_class_entry *syx_response_swoole_ce;

SYX_BEGIN_ARG_INFO_EX(syx_response_swoole_setfd_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, fd)
SYX_END_ARG_INFO()

PHP_METHOD(syx_response_swoole, getFd){
    syx_response_t *fd;
    fd = zend_read_property(syx_response_swoole_ce, getThis(), ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_FD), 0, NULL);
    RETURN_ZVAL(fd, 0, 0);
}

PHP_METHOD(syx_response_swoole, setFd){
    syx_response_t *fd;
    if(zend_parse_parameters_throw(ZEND_NUM_ARGS(), "a", &fd) == FAILURE){
        return;
    }
    zend_update_property(syx_response_swoole_ce, getThis(), ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_FD), fd);
    RETURN_TRUE;
}

zend_function_entry syx_response_swoole_methods[] = {
    PHP_ME(syx_response_swoole, getFd, syx_response_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_response_swoole, setFd, syx_response_swoole_setfd_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(response_swoole) {
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\Response\\SwooleAbstract", syx_response_swoole_methods);

    syx_response_swoole_ce = zend_register_internal_class_ex(&ce, syx_response_ce);
    syx_response_swoole_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
    zend_declare_property_long(syx_response_swoole_ce, ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_FD), 0, ZEND_ACC_PROTECTED);

    return SUCCESS;
}