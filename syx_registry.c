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
#include "syx_registry.h"

zend_class_entry *syx_registry_ce;

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(syx_registry_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_registry_has_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_registry_del_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_registry_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */

syx_registry_t *syx_registry_instance(syx_registry_t *this_ptr) /* {{{ */ {
	syx_registry_t *instance = zend_read_static_property(
			syx_registry_ce, ZEND_STRL(SYX_REGISTRY_PROPERTY_NAME_INSTANCE), 1);

	if (UNEXPECTED(Z_TYPE_P(instance) != IS_OBJECT ||
		!instanceof_function(Z_OBJCE_P(instance), syx_registry_ce))) {
		zval regs;

		object_init_ex(this_ptr, syx_registry_ce);

		array_init(&regs);
		zend_update_property(syx_registry_ce, this_ptr, ZEND_STRL(SYX_REGISTRY_PROPERTY_NAME_ENTRYS), &regs);
		zend_update_static_property(syx_registry_ce, ZEND_STRL(SYX_REGISTRY_PROPERTY_NAME_INSTANCE), this_ptr);
		zval_ptr_dtor(&regs);
		zval_ptr_dtor(this_ptr);

		instance = this_ptr;
	}

	return instance;
}
/* }}} */

int syx_registry_is_set(zend_string *name) /* {{{ */ {
	syx_registry_t *registry, rv;
	zval *entrys;

	registry = syx_registry_instance(&rv);
	entrys	= zend_read_property(syx_registry_ce, registry, ZEND_STRL(SYX_REGISTRY_PROPERTY_NAME_ENTRYS), 1, NULL);

	return zend_hash_exists(Z_ARRVAL_P(entrys), name);
}
/* }}} */

/** {{{ proto private Syx_Registry::__construct(void)
*/
PHP_METHOD(syx_registry, __construct) {
}
/* }}} */

/** {{{ proto private Syx_Registry::__clone(void)
*/
PHP_METHOD(syx_registry, __clone) {
}
/* }}} */

/** {{{ proto public static Syx_Registry::get($name)
*/
PHP_METHOD(syx_registry, get) {
	zend_string *name;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	} else {
		zval *pzval, *entrys;
		syx_registry_t *registry, rv;

		registry = syx_registry_instance(&rv);
		entrys = zend_read_property(syx_registry_ce, registry, ZEND_STRL(SYX_REGISTRY_PROPERTY_NAME_ENTRYS), 1, NULL);

		if (EXPECTED(entrys && Z_TYPE_P(entrys) == IS_ARRAY)) {
			if ((pzval = zend_hash_find(Z_ARRVAL_P(entrys), name)) != NULL) {
				RETURN_ZVAL(pzval, 1, 0);
			}
		}
	}

	RETURN_NULL();
}
/* }}} */

/** {{{ proto public static Syx_Registry::set($name, $value)
*/
PHP_METHOD(syx_registry, set) {
	zval *value;
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
		return;
	} else {
		syx_registry_t *registry, rv;
		zval *entrys;

		registry = syx_registry_instance(&rv);
		entrys = zend_read_property(syx_registry_ce, registry, ZEND_STRL(SYX_REGISTRY_PROPERTY_NAME_ENTRYS), 1, NULL);

		if (zend_hash_update(Z_ARRVAL_P(entrys), name, value) != NULL) {
			Z_TRY_ADDREF_P(value);
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public static Syx_Registry::del($name)
*/
PHP_METHOD(syx_registry, del) {
	zend_string *name;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	} else {
		syx_registry_t *registry, rv;
		zval *entrys;

		registry = syx_registry_instance(&rv);
		entrys = zend_read_property(syx_registry_ce, registry, ZEND_STRL(SYX_REGISTRY_PROPERTY_NAME_ENTRYS), 1, NULL);

		zend_hash_del(Z_ARRVAL_P(entrys), name);
	}

	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Registry::has($name)
*/
PHP_METHOD(syx_registry, has) {
	zend_string *name;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	} else {
		RETURN_BOOL(syx_registry_is_set(name));
	}
}
/* }}} */

/** {{{ proto public Syx_Registry::getInstance(void)
*/
PHP_METHOD(syx_registry, getInstance) {
	syx_registry_t *registry, rv = {{0}};
	registry = syx_registry_instance(&rv);
	RETURN_ZVAL(registry, 1, 0);
}
/* }}} */

/** {{{ syx_registry_methods
*/
zend_function_entry syx_registry_methods[] = {
	PHP_ME(syx_registry, __construct, 	NULL, ZEND_ACC_CTOR|ZEND_ACC_PRIVATE)
	PHP_ME(syx_registry, __clone, 		NULL, ZEND_ACC_CLONE|ZEND_ACC_PRIVATE)
	PHP_ME(syx_registry, get, syx_registry_get_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(syx_registry, has, syx_registry_has_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(syx_registry, set, syx_registry_set_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(syx_registry, del, syx_registry_del_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(registry) {
	zend_class_entry ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Registry", syx_registry_methods);

	syx_registry_ce = zend_register_internal_class_ex(&ce, NULL);
	syx_registry_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(syx_registry_ce, ZEND_STRL(SYX_REGISTRY_PROPERTY_NAME_INSTANCE), ZEND_ACC_PROTECTED|ZEND_ACC_STATIC);
	zend_declare_property_null(syx_registry_ce, ZEND_STRL(SYX_REGISTRY_PROPERTY_NAME_ENTRYS),  ZEND_ACC_PROTECTED);

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
