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
#include "syx_exception.h"

#include "syx_server.h"
#include "server/syx_server_interface.h"
#include "server/syx_server_plugin.h"
#include "server/syx_server_udp.h"

zend_class_entry *syx_server_udp_ce;

void syx_server_swoole_onPacket(syx_server_t* syx_swoole_server_o){
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL("packet"), ZEND_STRL("Syx\\ServerAbstract::packet"));
}

PHP_METHOD(syx_server_udp, start){
    RETURN_TRUE;
}

PHP_METHOD(syx_server_udp, packet){
    RETURN_TRUE;
}

/** {{{ syx_server_abstract_methods
 */
zend_function_entry syx_server_udp_methods[] = {
    PHP_ME(syx_server_udp, start, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_server_udp, packet, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
 */
SYX_STARTUP_FUNCTION(server_udp) {
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\Server\\Udp", syx_server_udp_methods);
    syx_server_udp_ce = zend_register_internal_class_ex(&ce, syx_server_ce);

    return SUCCESS;
}