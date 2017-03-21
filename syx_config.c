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
#include "php_ini.h"
#include "standard/php_filestat.h" /* for php_stat */

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_exception.h"
#include "syx_config.h"

#include "configs/syx_config_ini.h"
#include "configs/syx_config_simple.h"

zend_class_entry *syx_config_ce;

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(syx_config_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()
/* }}} */

syx_config_t *syx_config_instance(syx_config_t *this_ptr, zval *arg1, zval *arg2) /* {{{ */ {
	syx_config_t *instance;

	if (!arg1) {
		return NULL;
	}

	if (Z_TYPE_P(arg1) == IS_STRING) {
		if (strncasecmp(Z_STRVAL_P(arg1) + Z_STRLEN_P(arg1) - 3, "ini", 3) == 0) {
			instance = syx_config_ini_instance(this_ptr, arg1, arg2);
			if (!instance) {
				return NULL;
			}

			return instance;
		}
		syx_trigger_error(SYX_ERR_TYPE_ERROR, "Expects a path to *.ini configuration file as parameter");
		return NULL;
	} else if (Z_TYPE_P(arg1) == IS_ARRAY) {
		zval readonly;
		ZVAL_BOOL(&readonly, 1);
		instance = syx_config_simple_instance(this_ptr, arg1, &readonly);
		return instance;
	}

	syx_trigger_error(SYX_ERR_TYPE_ERROR, "Expects a string or an array as parameter");
	return NULL;
}
/* }}} */

/** {{{ syx_config_methods
*/
zend_function_entry syx_config_methods[] = {
	PHP_ABSTRACT_ME(syx_config, get, NULL)
	PHP_ABSTRACT_ME(syx_config, set, NULL)
	PHP_ABSTRACT_ME(syx_config, readonly, NULL)
	PHP_ABSTRACT_ME(syx_config, toArray, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(config) {
	zend_class_entry ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\ConfigAbstract", syx_config_methods);
	syx_config_ce = zend_register_internal_class_ex(&ce, NULL);
	syx_config_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(syx_config_ce, ZEND_STRL(SYX_CONFIG_PROPERT_NAME), ZEND_ACC_PROTECTED);
	zend_declare_property_bool(syx_config_ce, ZEND_STRL(SYX_CONFIG_PROPERT_NAME_READONLY), 1, ZEND_ACC_PROTECTED);

	SYX_STARTUP(config_ini);
	SYX_STARTUP(config_simple);

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
