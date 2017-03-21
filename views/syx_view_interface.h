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

#ifndef SYX_VIEW_INTERFACE_H
#define SYX_VIEW_INTERFACE_H

SYX_BEGIN_ARG_INFO_EX(syx_view_assign_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_view_display_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, tpl)
	ZEND_ARG_INFO(0, tpl_vars)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_view_render_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, tpl)
	ZEND_ARG_INFO(0, tpl_vars)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_view_setpath_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, template_dir)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_view_getpath_arginfo, 0, 0, 0)
SYX_END_ARG_INFO()

extern zend_class_entry *syx_view_interface_ce;

SYX_STARTUP_FUNCTION(view_interface);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
