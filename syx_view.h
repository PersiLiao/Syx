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

#ifndef SYX_VIEW_H
#define SYX_VIEW_H

#define syx_view_instance syx_view_simple_instance
#define syx_view_ce		  syx_view_simple_ce

#define SYX_VIEW_PROPERTY_NAME_TPLVARS 	"_tpl_vars"
#define SYX_VIEW_PROPERTY_NAME_TPLDIR	"_tpl_dir"
#define SYX_VIEW_PROPERTY_NAME_OPTS 	"_options"

extern zend_class_entry *syx_view_interface_ce;
extern zend_class_entry *syx_view_simple_ce;

syx_view_t * syx_view_instance(syx_view_t * this_ptr, zval *tpl_dir, zval *options);
int syx_view_simple_render(syx_view_t *view, zval *tpl, zval * vars, zval *ret);
int syx_view_simple_display(syx_view_t *view, zval *tpl, zval * vars, zval *ret);
int syx_view_simple_assign_multi(syx_view_t *view, zval *value);
void syx_view_simple_clear_assign(syx_view_t *view, zend_string *name);

SYX_STARTUP_FUNCTION(view);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
