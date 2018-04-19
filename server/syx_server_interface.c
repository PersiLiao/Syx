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
#include "server/syx_server_interface.h"

zend_class_entry *syx_server_interface_ce;

/** {{{ syx_server_interface_methods
 */
zend_function_entry syx_server_interface_methods[] = {
    PHP_ABSTRACT_ME(syx_server_interface, start, syx_server_interface_void_arginfo)
    PHP_ABSTRACT_ME(syx_server_interface, reload, syx_server_interface_reload_arginfo)
    PHP_ABSTRACT_ME(syx_server_interface, stop, syx_server_interface_void_arginfo)
    PHP_ABSTRACT_ME(syx_server_interface, setDaemonize, syx_server_interface_void_arginfo)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
 */
SYX_STARTUP_FUNCTION(server_interface) {
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\ServerInterface", syx_server_interface_methods);
    syx_server_interface_ce = zend_register_internal_interface(&ce);

    return SUCCESS;
}