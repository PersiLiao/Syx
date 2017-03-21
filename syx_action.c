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
#include "syx_request.h"
#include "syx_response.h"
#include "syx_view.h"
#include "syx_exception.h"
#include "syx_controller.h"
#include "syx_action.h"

zend_class_entry *syx_action_ce;

/** {{{ ARG_INFO
 */

/* }}} */

/** {{{ proto public Syx_Action_Abstract::getController(void)
*/
PHP_METHOD(syx_action, getController) {
	syx_controller_t *controller = zend_read_property(syx_action_ce,
			getThis(), ZEND_STRL(SYX_ACTION_PROPERTY_NAME_CTRL), 1, NULL);
	RETURN_ZVAL(controller, 1, 0);
}
/* }}} */

/** {{{ syx_controller_methods
*/
zend_function_entry syx_action_methods[] = {
	PHP_ABSTRACT_ME(syx_action_controller, execute, NULL)
	PHP_ME(syx_action, getController, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(action) {
	zend_class_entry ce;
	SYX_INIT_CLASS_ENTRY(ce, "Syx\\ActionAbstract", syx_action_methods);
	syx_action_ce = zend_register_internal_class_ex(&ce, syx_controller_ce);
	syx_action_ce->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(syx_action_ce, ZEND_STRL(SYX_ACTION_PROPERTY_NAME_CTRL),	ZEND_ACC_PROTECTED);

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
