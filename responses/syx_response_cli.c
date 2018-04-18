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
#include "syx_response.h"
#include "syx_exception.h"

#include "responses/syx_response_cli.h"

zend_class_entry * syx_response_cli_ce;

/** {{{ proto public Syx_Response_Abstract::response(void)
 */
PHP_METHOD(response_cli, response) {
    zval *zbody;
    zval *val;

    zbody = zend_read_property(syx_response_ce, getThis(), ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zbody), val) {
        if (UNEXPECTED(Z_TYPE_P(val) != IS_STRING)) {
            continue;
        }
        php_write(Z_STRVAL_P(val), Z_STRLEN_P(val));
    } ZEND_HASH_FOREACH_END();
    RETURN_TRUE;
}
/* }}} */

/** {{{ syx_response_methods
*/
zend_function_entry syx_response_cli_methods[] = {
    PHP_ME(response_cli, response,      syx_response_void_arginfo,              ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(response_cli) {
	zend_class_entry ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Response\\Cli", syx_response_cli_methods);

	syx_response_cli_ce = zend_register_internal_class_ex(&ce, syx_response_ce);

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
