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
#include "syx_exception.h"
#include "syx_loader.h"
#include "syx_view.h"

#include "views/syx_view_interface.h"

zend_class_entry *syx_view_interface_ce;

/** {{{ syx_view_interface_methods
 */
zend_function_entry syx_view_interface_methods[] = {
	ZEND_ABSTRACT_ME(syx_view, assign,  syx_view_assign_arginfo)
	ZEND_ABSTRACT_ME(syx_view, display, syx_view_display_arginfo)
	ZEND_ABSTRACT_ME(syx_view, render, syx_view_render_arginfo)
	ZEND_ABSTRACT_ME(syx_view, setScriptPath, syx_view_setpath_arginfo)
	ZEND_ABSTRACT_ME(syx_view, getScriptPath, syx_view_getpath_arginfo)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
 */
SYX_STARTUP_FUNCTION(view_interface) {
	zend_class_entry ce;
	SYX_INIT_CLASS_ENTRY(ce, "Syx\\ViewInterface", syx_view_interface_methods);
	syx_view_interface_ce = zend_register_internal_interface(&ce);

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

