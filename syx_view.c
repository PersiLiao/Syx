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
#include "views/syx_view_simple.h"

/** {{{ syx_view_t * syx_view_instance(syx_view_t *this_ptr)
*/
#if 0
syx_view_t * syx_view_instance(syx_view_t *this_ptr) {
	syx_view_t		*view	= NULL;
	syx_view_struct 	*p 		= syx_buildin_views;

	for(;;++p) {
		syx_current_view = p;
		syx_view_ce = *(p->ce);
		break;
	}

	syx_view_ce = *(syx_current_view->ce);

	MAKE_STD_ZVAL(view);
	object_init_ex(view, *(syx_current_view->ce));

	if (syx_current_view->init) {
		syx_current_view->init(view);
	}
	MAKE_STD_ZVAL(view);
	object_init_ex(view, syx_view_simple_ce);
	syx_view_simple_init(view);

	return view;
}
#endif
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(view) {
	/* tricky way to supress warning "defined but not used" */
	(void)syx_view_assign_arginfo;
	(void)syx_view_display_arginfo;
	(void)syx_view_render_arginfo;
	(void)syx_view_getpath_arginfo;
	(void)syx_view_setpath_arginfo;

	SYX_STARTUP(view_interface);
	SYX_STARTUP(view_simple);

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
