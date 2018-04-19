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
#include "main/SAPI.h" /* for sapi_header_line */
#include "ext/standard/php_string.h" /* for php_implode */
#include "Zend/zend_interfaces.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_response.h"
#include "syx_dispatcher.h"
#include "syx_exception.h"

#include "server/syx_server_interface.h"
#include "responses/syx_response_http.h"
#include "responses/syx_response_cli.h"
#include "responses/syx_response_swoole.h"
#include "responses/syx_response_swoole_tcp.h"
#include "responses/syx_response_swoole_http.h"
#include "responses/syx_response_swoole_websocket.h"

zend_class_entry *syx_response_ce;

/** {{{ ARG_INFO
 */
SYX_BEGIN_ARG_INFO_EX(syx_response_get_body_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, name)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_set_body_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, body)
	SYX_ARG_INFO(0, name)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_clear_body_arginfo, 0, 0, 1)
	SYX_ARG_INFO(0, name)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_set_response_type_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, type)
SYX_END_ARG_INFO()

/* }}} */

/** {{{ syx_response_t * syx_response_instance(syx_response_t *this_ptr, char *sapi_name)
 */
syx_response_t * syx_response_instance(syx_dispatcher_t *syx_dispatcher, syx_response_t *this_ptr) {
	zval header, body, cookie = {{0}};
	zend_class_entry 	*ce;
	syx_response_t 		*instance;
	uint sapi_len;
	char *sapi_name = NULL;

	sapi_name = SYX_G(sapi);
	if(sapi_name == NULL || strlen(sapi_name) == 0){
	    sapi_name = sapi_module.name;
	}

	if(strncasecmp(sapi_name, SYX_SERVER_TYPE_SWOOLE, strlen(SYX_SERVER_TYPE_SWOOLE)) == 0){
	    if(strncasecmp(sapi_name, SYX_SERVER_TYPE_SWOOLE_TCP, strlen(SYX_SERVER_TYPE_SWOOLE_TCP)) == 0 && instanceof_function(Z_OBJCE_P(this_ptr), syx_response_swoole_tcp_ce)){
	        return this_ptr;
	    }else if(strncasecmp(sapi_name, SYX_SERVER_TYPE_SWOOLE_HTTP, strlen(SYX_SERVER_TYPE_SWOOLE_HTTP)) == 0 && instanceof_function(Z_OBJCE_P(this_ptr), syx_response_swoole_http_ce)){
	        array_init(&header);
            zend_update_property(syx_response_swoole_http_ce, this_ptr, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_HEADER), &header);
            zval_ptr_dtor(&header);
            array_init(&cookie);
            zend_update_property(syx_response_swoole_http_ce, this_ptr, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_COOKIE), &cookie);
            zval_ptr_dtor(&cookie);
            return this_ptr;
	    }else if(strncasecmp(sapi_name, SYX_SERVER_TYPE_SWOOLE_WEBSOCKET, strlen(SYX_SERVER_TYPE_SWOOLE_WEBSOCKET)) == 0 && instanceof_function(Z_OBJCE_P(this_ptr), syx_response_swoole_websocket_ce)){
	        return this_ptr;
	    }
	}else{
	    if (strncasecmp(sapi_name, "cli", 3)) {
            ce = syx_response_http_ce;
        } else {
            ce = syx_response_cli_ce;
        }
	    if (Z_ISUNDEF_P(this_ptr)) {
            object_init_ex(this_ptr, ce);
            array_init(&header);
            zend_update_property(ce, this_ptr, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_HEADER), &header);
            zval_ptr_dtor(&header);
            array_init(&cookie);
            zend_update_property(syx_response_swoole_http_ce, this_ptr, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_COOKIE), &cookie);
            zval_ptr_dtor(&cookie);
        }
	}
	return this_ptr;
}
/* }}} */

/** {{{ static int syx_response_set_body(syx_response_t *response, char *name, int name_len, char *body, long body_len)
 */

static int syx_response_set_body(syx_response_t *response, char *name, int name_len, char *body, long body_len) {
	zval *zbody;
	zend_class_entry *response_ce;

	if (!body_len) {
		return 1;
	}

	response_ce = Z_OBJCE_P(response);

	zbody = zend_read_property(response_ce, response, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);
	if(!ZVAL_IS_NULL(zbody)){
	    zval_ptr_dtor(&zbody);
	}
	ZVAL_STRINGL(zbody, body, body_len);
	zend_update_property(response_ce, response, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), zbody);
	zval_ptr_dtor(zbody);
	return 1;
}
/* }}} */

/** {{{ int syx_response_alter_body(syx_response_t *response, zend_string *name, zend_string *body, int flag)
 */
int syx_response_alter_body(syx_response_t *response, zend_string *name, zend_string *body, int flag) {
	zval *zbody, *pzval;
	uint  free_name = 0;
	zend_string *obody;

	if (ZSTR_LEN(body) == 0) {
		return 1;
	}

	zbody = zend_read_property(syx_response_ce, response, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);
	if (!name) {
		name = zend_string_init(ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_DEFAULTBODY), 0);
		free_name = 1;
	}

	if ((pzval = zend_hash_find(Z_ARRVAL_P(zbody), name)) == NULL) {
		obody = NULL;
		pzval = zend_hash_update(Z_ARRVAL_P(zbody), name, &EG(uninitialized_zval));
	} else {
		obody = Z_STR_P(pzval);
	}

	if (obody == NULL) {
		ZVAL_STR_COPY(pzval, body);
	} else {
		zend_string *result;
		size_t result_len;

		switch (flag) {
			case SYX_RESPONSE_PREPEND:
				result_len = ZSTR_LEN(body) + ZSTR_LEN(obody);
				result = zend_string_alloc(result_len, 0);
				memcpy(ZSTR_VAL(result), ZSTR_VAL(body), ZSTR_LEN(body));
				memcpy(ZSTR_VAL(result) + ZSTR_LEN(body), ZSTR_VAL(obody), ZSTR_LEN(obody) + 1);
				zend_string_release(obody);
				ZVAL_NEW_STR(pzval, result);
				break;
			case SYX_RESPONSE_APPEND:
				result_len = ZSTR_LEN(body) + ZSTR_LEN(obody);
				result = zend_string_realloc(obody, result_len, 0);
				memcpy(ZSTR_VAL(result) + ZSTR_LEN(result) - ZSTR_LEN(body), ZSTR_VAL(body), ZSTR_LEN(body) + 1);
				ZVAL_NEW_STR(pzval, result);
				break;
			case SYX_RESPONSE_REPLACE:
			default:
				zend_string_release(obody);
				ZVAL_STR_COPY(pzval, body);
				break;
		}
	}

	if (free_name) {
		zend_string_release(name);
	}

	return 1;
}
/* }}} */

/** {{{ int syx_response_clear_body(syx_response_t *response, zend_string *name)
 */
int syx_response_clear_body(syx_response_t *response, zend_string *name) {
	zval *zbody;
	zbody = zend_read_property(syx_response_ce, response, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);

	if (name) {
		zend_hash_del(Z_ARRVAL_P(zbody), name);
	} else {
		zend_hash_clean(Z_ARRVAL_P(zbody));
	}
	return 1;
}
/* }}} */

/** {{{ zval * syx_response_get_body(syx_response_t *response, zend_string *name)
 */
zval * syx_response_get_body(syx_response_t *response, zend_string *name) {
	zval *zbody = zend_read_property(syx_response_ce,
			response, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);

	if (!name) {
		return zbody;
	}

	return zend_hash_find(Z_ARRVAL_P(zbody), name);
}
/* }}} */

/** {{{ zval * syx_response_get_body_str(syx_response_t *response, char *name, size_t len)
 */
zval * syx_response_get_body_str(syx_response_t *response, char *name, size_t len) {
	zval *ret;
	zend_string *n = zend_string_init(name, len, 0);
	ret = syx_response_get_body(response, n);
	zend_string_release(n);
	return ret;
}
/* }}} */

/** {{{ int syx_response_send(syx_response_t *response)
 */

/* }}} */

/** {{{ proto private Syx_Response_Abstract::__construct()
*/
PHP_METHOD(syx_response, __construct) {
	(void)syx_response_instance(zend_read_static_property(syx_dispatcher_ce, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_INSTANCE), 0), getThis());
}
/* }}} */

/** {{{ proto public Syx_Response_Abstract::__destruct(void)
*/
PHP_METHOD(syx_response, __destruct) {
}
/* }}} */

/** {{{ proto public Syx_Response_Abstract::appendBody($body, $name = NULL)
*/
PHP_METHOD(syx_response, appendBody) {
	zend_string *name = NULL;
	zend_string *body;
	syx_response_t *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &body, &name) == FAILURE) {
		return;
	}

	self = getThis();

	if (syx_response_alter_body(self, name, body, SYX_RESPONSE_APPEND)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Response_Abstract::prependBody($body, $name = NULL)
*/
PHP_METHOD(syx_response, prependBody) {
	zend_string	*name = NULL;
	zend_string *body;
	syx_response_t *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &body, &name) == FAILURE) {
		return;
	}

	self = getThis();

	if (syx_response_alter_body(self, name, body, SYX_RESPONSE_PREPEND)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Response_Abstract::setBody($body, $name = NULL)
*/
PHP_METHOD(syx_response, setBody) {
    zend_string *body;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &body) == FAILURE) {
		return;
	}
	zend_update_property_str(syx_response_ce, getThis(), ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), body);
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Response_Abstract::clearBody(string $name = NULL)
*/
PHP_METHOD(syx_response, clearBody) {
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}
	if (syx_response_clear_body(getThis(), name)) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Response_Abstract::getBody(string $name = NULL)
 */
PHP_METHOD(syx_response, getBody) {
	zval *body;
	zval *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &name) == FAILURE) {
		return;
	}

	if (!name) {
		body = syx_response_get_body_str(getThis(),
				SYX_RESPONSE_PROPERTY_NAME_DEFAULTBODY, sizeof(SYX_RESPONSE_PROPERTY_NAME_DEFAULTBODY) - 1);
	} else {
		if (ZVAL_IS_NULL(name)) {
			body = syx_response_get_body(getThis(), NULL);
		} else {
			convert_to_string_ex(name);
			body = syx_response_get_body(getThis(), Z_STR_P(name));
		}
	}

	if (body) {
		RETURN_ZVAL(body, 1, 0);
	}

	RETURN_EMPTY_STRING();
}
/* }}} */

PHP_METHOD(syx_response, setResponseType){
    syx_response_t *level;
    if(zend_parse_parameters_throw(ZEND_NUM_ARGS(), "l", &level) == FAILURE){
        return;
    }
    zend_update_property_long(syx_response_ce, getThis(), ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_SEND_TYPE), level);
    RETURN_TRUE;
}

/** {{{ proto public Syx_Response_Abstract::__toString(void)
 */
PHP_METHOD(syx_response, __toString) {
	zend_string *delim;
	zval *zbody = zend_read_property(syx_response_ce,
			getThis(), ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);

	delim = ZSTR_EMPTY_ALLOC();
	php_implode(delim, zbody, return_value);
	zend_string_release(delim);
}
/* }}} */

PHP_METHOD(syx_response, setFile){
    RETURN_TRUE;
}

/** {{{ proto public Syx_Response_Abstract::__clone(void)
*/
PHP_METHOD(syx_response, __clone) {
}
/* }}} */

/** {{{ syx_response_methods
*/
zend_function_entry syx_response_methods[] = {
	PHP_ME(syx_response, __destruct,  	syx_response_void_arginfo, 		    ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	PHP_ME(syx_response, __clone,		syx_response_void_arginfo, 		    ZEND_ACC_PRIVATE)
	PHP_ME(syx_response, __toString,	syx_response_void_arginfo, 		    ZEND_ACC_PUBLIC)
	PHP_ME(syx_response, setBody,		syx_response_set_body_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(syx_response, appendBody,	syx_response_set_body_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(syx_response, prependBody,	syx_response_set_body_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(syx_response, clearBody,		syx_response_clear_body_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(syx_response, getBody,		syx_response_get_body_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(syx_response, setResponseType,  syx_response_set_response_type_arginfo,      ZEND_ACC_PUBLIC)
	PHP_ME(syx_response, setFile,  syx_response_set_response_type_arginfo,      ZEND_ACC_PUBLIC)
	PHP_ABSTRACT_ME(syx_response, response,	syx_response_void_arginfo)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(response) {
	zend_class_entry ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\ResponseAbstract", syx_response_methods);

	syx_response_ce = zend_register_internal_class_ex(&ce, NULL);
	syx_response_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(syx_response_ce, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_HEADER), ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_response_ce, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), ZEND_ACC_PROTECTED);
	zend_declare_class_constant_stringl(syx_response_ce, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_DEFAULTBODYNAME), ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_DEFAULTBODY));
	zend_declare_property_long(syx_response_ce, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_SEND_TYPE), SYX_RESPONSE_CONSTANT_NAME_SIMPLE_VALUE, ZEND_ACC_PROTECTED);

	zend_declare_class_constant_long(syx_response_ce, ZEND_STRL(SYX_RESPONSE_CONSTANT_NAME_SIMPLE), SYX_RESPONSE_CONSTANT_NAME_SIMPLE_VALUE);
	zend_declare_class_constant_long(syx_response_ce, ZEND_STRL(SYX_RESPONSE_CONSTANT_NAME_TEXT), SYX_RESPONSE_CONSTANT_NAME_TEXT_VALUE);
	zend_declare_class_constant_long(syx_response_ce, ZEND_STRL(SYX_RESPONSE_CONSTANT_NAME_FILE), SYX_RESPONSE_CONSTANT_NAME_FILE_VALUE);
	zend_declare_class_constant_long(syx_response_ce, ZEND_STRL(SYX_RESPONSE_CONSTANT_NAME_BINARY), SYX_RESPONSE_CONSTANT_NAME_BINARY_VALUE);

	SYX_STARTUP(response_http);
	SYX_STARTUP(response_cli);
	SYX_STARTUP(response_swoole);
	SYX_STARTUP(response_swoole_tcp);
	SYX_STARTUP(response_swoole_http);
	SYX_STARTUP(response_swoole_websocket);

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
