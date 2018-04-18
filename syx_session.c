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
#include "Zend/zend_interfaces.h"  /* for zend_ce_iterator */

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_session.h"
#include "syx_exception.h"

zend_class_entry *syx_session_ce;

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

/* {{{ ARG_INFO
 */
SYX_BEGIN_ARG_INFO_EX(syx_session_void_arginfo, 0, 0, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_session_get_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, name)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_session_has_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, name)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_session_del_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, name)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_session_set_arginfo, 0, 0, 2)
	SYX_ARG_INFO(0, name)
	SYX_ARG_INFO(0, value)
SYX_END_ARG_INFO()
/* }}} */

static inline void syx_session_start(syx_session_t *session) /* {{{ */ {
	zval *status;

	status = zend_read_property(syx_session_ce, session, ZEND_STRL(SYX_SESSION_PROPERTY_NAME_STATUS), 1, NULL);
	if (Z_TYPE_P(status) == IS_TRUE) {
		return;
	}

	php_session_start();
	zend_update_property_bool(syx_session_ce, session, ZEND_STRL(SYX_SESSION_PROPERTY_NAME_STATUS), 1);
	return;
}
/* }}} */

static syx_session_t *syx_session_instance(syx_session_t *this_ptr) /* {{{ */ {
	zval *sess, member;
	zend_object *obj;
	zend_property_info *property_info;

	object_init_ex(this_ptr, syx_session_ce);

	syx_session_start(this_ptr);

	if ((sess = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_SESSION"))) == NULL
			|| Z_TYPE_P(sess) != IS_REFERENCE
			|| Z_TYPE_P(Z_REFVAL_P(sess)) != IS_ARRAY) {
		php_error_docref(NULL, E_WARNING, "Attempt to start session failed");
		zval_ptr_dtor(this_ptr);
		return NULL;
	}

	ZVAL_STRING(&member, SYX_SESSION_PROPERTY_NAME_SESSION);

	obj = Z_OBJ_P(this_ptr);

	property_info = zend_get_property_info(obj->ce, Z_STR(member), 1);

	zval_ptr_dtor(&member);

	if (property_info->offset != ZEND_WRONG_PROPERTY_OFFSET) {
		zval *prop = OBJ_PROP(obj, property_info->offset);
		ZVAL_COPY(prop, sess);
	}

	zend_update_static_property(syx_session_ce, ZEND_STRL(SYX_SESSION_PROPERTY_NAME_INSTANCE), this_ptr);

	return this_ptr;
}
/* }}} */

/** {{{ proto private Syx_Session::__construct(void)
*/
PHP_METHOD(syx_session, __construct) {
}
/* }}} */

/** {{{ proto private Syx_Session::__destruct(void)
*/
PHP_METHOD(syx_session, __destruct) {
}
/* }}} */

/** {{{ proto private Syx_Session::__sleep(void)
*/
PHP_METHOD(syx_session, __sleep) {
}
/* }}} */

/** {{{ proto private Syx_Session::__wakeup(void)
*/
PHP_METHOD(syx_session, __wakeup) {
}
/* }}} */

/** {{{ proto private Syx_Session::__clone(void)
*/
PHP_METHOD(syx_session, __clone) {
}
/* }}} */

/** {{{ proto public Syx_Session::getInstance(void)
*/
PHP_METHOD(syx_session, getInstance) {
	syx_session_t *instance;

	instance = zend_read_static_property(syx_session_ce, ZEND_STRL(SYX_SESSION_PROPERTY_NAME_INSTANCE), 1);

	if (Z_TYPE_P(instance) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(instance), syx_session_ce)) {
		zval rv = {{0}};
		if ((instance = syx_session_instance(&rv))) {
			RETURN_ZVAL(instance, 0, 0);
		} else {
			RETURN_NULL();
		}
	} else {
		RETURN_ZVAL(instance, 1, 0);
	}
}
/* }}} */

/** {{{ proto public Syx_Session::count(void)
*/
PHP_METHOD(syx_session, count) {
	zval *sess = zend_read_property(syx_session_ce, getThis(), ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(Z_REFVAL_P(sess))));
}
/* }}} */

/** {{{ proto public static Syx_Session::start()
*/
PHP_METHOD(syx_session, start) {
	syx_session_start(getThis());
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public static Syx_Session::get($name)
*/
PHP_METHOD(syx_session, get) {
	zend_string *name 	= NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		zval *ret, *sess;

		sess = zend_read_property(syx_session_ce, getThis(), ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);
		if (!name) {
			RETURN_ZVAL(sess, 1, 0);
		}

		if ((ret = zend_hash_find(Z_ARRVAL_P(Z_REFVAL_P(sess)), name)) == NULL ){
			RETURN_NULL();
		}

		RETURN_ZVAL(ret, 1, 0);
	}
}
/* }}} */

/** {{{ proto public static Syx_Session::set($name, $value)
*/
PHP_METHOD(syx_session, set) {
	zval *value;
	zend_string *name;
	zval *self = getThis();
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
		return;
	} else {
		zval *sess = zend_read_property(syx_session_ce, self, ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);
		if (zend_hash_update(Z_ARRVAL_P(Z_REFVAL_P(sess)), name, value) == NULL) {
			RETURN_FALSE;
		}
		Z_TRY_ADDREF_P(value);
	}

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public static Syx_Session::del($name)
*/
PHP_METHOD(syx_session, del) {
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	} else {
		zval *sess = zend_read_property(syx_session_ce, getThis(), ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);

		if (zend_hash_del(Z_ARRVAL_P(Z_REFVAL_P(sess)), name) == SUCCESS) {
			RETURN_ZVAL(getThis(), 1, 0);
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public static Syx_Session::clear()
*/
PHP_METHOD(syx_session, clear) {
	zval *sess = zend_read_property(syx_session_ce, getThis(), ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);

	zend_hash_clean(Z_ARRVAL_P(Z_REFVAL_P(sess)));
}
/* }}} */

/** {{{ proto public Syx_Session::has($name)
*/
PHP_METHOD(syx_session, has) {
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	} else {
		zval *sess = zend_read_property(syx_session_ce,
				getThis(), ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);
		RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(Z_REFVAL_P(sess)), name));
	}

}
/* }}} */

/** {{{ proto public Syx_Session::rewind(void)
*/
PHP_METHOD(syx_session, rewind) {
	zval *sess = zend_read_property(syx_session_ce,
			getThis(), ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(Z_REFVAL_P(sess)));
}
/* }}} */

/** {{{ proto public Syx_Session::current(void)
*/
PHP_METHOD(syx_session, current) {
	zval *sess, *pzval;
	sess = zend_read_property(syx_session_ce,
			getThis(), ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);
	if ((pzval = zend_hash_get_current_data(Z_ARRVAL_P(Z_REFVAL_P(sess)))) == NULL) {
		RETURN_FALSE;
	}

	RETURN_ZVAL(pzval, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Session::key(void)
*/
PHP_METHOD(syx_session, key) {
	zval *sess;
	zend_string *key;
	ulong index;

	sess = zend_read_property(syx_session_ce,
			getThis(), ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);
	if (zend_hash_get_current_key(Z_ARRVAL_P(Z_REFVAL_P(sess)), &key, &index) == HASH_KEY_IS_LONG) {
		RETURN_LONG(index);
	} else {
		RETURN_STR(zend_string_copy(key));
	}
}
/* }}} */

/** {{{ proto public Syx_Session::next(void)
*/
PHP_METHOD(syx_session, next) {
	zval *sess = zend_read_property(syx_session_ce,
			getThis(), ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);
	zend_hash_move_forward(Z_ARRVAL_P(Z_REFVAL_P(sess)));
}
/* }}} */

/** {{{ proto public Syx_Session::valid(void)
*/
PHP_METHOD(syx_session, valid) {
	zval *sess = zend_read_property(syx_session_ce,
			getThis(), ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION), 1, NULL);
	RETURN_BOOL(zend_hash_has_more_elements(Z_ARRVAL_P(Z_REFVAL_P(sess))) == SUCCESS);
}
/* }}} */

/** {{{ syx_session_methods
*/
zend_function_entry syx_session_methods[] = {
	PHP_ME(syx_session, __construct, NULL, ZEND_ACC_CTOR|ZEND_ACC_PRIVATE)
	PHP_ME(syx_session, __clone, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(syx_session, __sleep, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(syx_session, __wakeup, NULL, ZEND_ACC_PRIVATE)
	PHP_ME(syx_session, getInstance, syx_session_void_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(syx_session, start, syx_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, get, syx_session_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, has, syx_session_has_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, set, syx_session_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, del, syx_session_del_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, count, syx_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, rewind, syx_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, next, syx_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, current, syx_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, key, syx_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, valid, syx_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_session, clear, syx_session_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_session, offsetGet, get, syx_session_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_session, offsetSet, set, syx_session_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_session, offsetExists, has, syx_session_has_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_session, offsetUnset, del, syx_session_del_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_session, __get, get, syx_session_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_session, __isset, has, syx_session_has_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_session, __set, set, syx_session_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_session, __unset, del, syx_session_del_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(session) {
	zend_class_entry ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Session", syx_session_methods);

	syx_session_ce = zend_register_internal_class_ex(&ce, NULL);
	syx_session_ce->ce_flags |= ZEND_ACC_FINAL;

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
	zend_class_implements(syx_session_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#elif PHP_VERSION_ID >= 70200
	zend_class_implements(syx_session_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, zend_ce_countable);
#endif

	zend_declare_property_null(syx_session_ce, ZEND_STRL(SYX_SESSION_PROPERTY_NAME_INSTANCE), ZEND_ACC_PROTECTED|ZEND_ACC_STATIC);
	zend_declare_property_null(syx_session_ce, ZEND_STRL(SYX_SESSION_PROPERTY_NAME_SESSION),  ZEND_ACC_PROTECTED);
	zend_declare_property_bool(syx_session_ce, ZEND_STRL(SYX_SESSION_PROPERTY_NAME_STATUS),   0, ZEND_ACC_PROTECTED);

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
