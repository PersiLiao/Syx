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

#ifndef SYX_REQUEST_H
#define SYX_REQUEST_H

#define SYX_REQUEST_PROPERTY_CONTROLLER_MAXLEN 128

#define SYX_REQUEST_PROPERTY_NAME_MODULE     "module"
#define SYX_REQUEST_PROPERTY_NAME_CONTROLLER "controller"
#define SYX_REQUEST_PROPERTY_NAME_ACTION     "action"
#define SYX_REQUEST_PROPERTY_NAME_METHOD     "method"
#define SYX_REQUEST_PROPERTY_NAME_PARAMS     "params"
#define SYX_REQUEST_PROPERTY_NAME_URI        "uri"
#define SYX_REQUEST_PROPERTY_NAME_STATE      "dispatched"
#define SYX_REQUEST_PROPERTY_NAME_LANG       "language"
#define SYX_REQUEST_PROPERTY_NAME_ROUTED     "routed"
#define SYX_REQUEST_PROPERTY_NAME_BASE       "_base_uri"
#define SYX_REQUEST_PROPERTY_NAME_EXCEPTION  "_exception"

#define SYX_REQUEST_SERVER_URI               "request_uri="

#define SYX_GLOBAL_VARS_TYPE                 unsigned int
#define SYX_GLOBAL_VARS_POST                 TRACK_VARS_POST
#define SYX_GLOBAL_VARS_GET                  TRACK_VARS_GET
#define SYX_GLOBAL_VARS_ENV                  TRACK_VARS_ENV
#define SYX_GLOBAL_VARS_FILES                TRACK_VARS_FILES
#define SYX_GLOBAL_VARS_SERVER               TRACK_VARS_SERVER
#define SYX_GLOBAL_VARS_REQUEST              TRACK_VARS_REQUEST
#define SYX_GLOBAL_VARS_COOKIE               TRACK_VARS_COOKIE

extern zend_class_entry *syx_request_ce;

extern PHPAPI void php_session_start();

syx_request_t *syx_request_instance(syx_request_t *this_ptr, zend_string *info);
int syx_request_set_base_uri(syx_request_t *request, zend_string *base_uri, zend_string *request_uri);
zval *syx_request_query_ex(uint type, zend_bool fetch_type, void *name, size_t len);

zval *syx_request_get_method(syx_request_t *instance);
zval *syx_request_get_param(syx_request_t *instance, zend_string *key);
zval *syx_request_get_language(syx_request_t *instance, zval *accept_language);

int syx_request_is_routed(syx_request_t *request);
int syx_request_is_dispatched(syx_request_t *request);
void syx_request_set_dispatched(syx_request_t *request, int flag);
void syx_request_set_routed(syx_request_t *request, int flag);
int syx_request_set_params_single(syx_request_t *instance, zend_string *key, zval *value);
int syx_request_set_params_multi(syx_request_t *instance, zval *values);

#define syx_request_query(type, name)  syx_request_query_ex((type), 1, (name), 0)
#define syx_request_query_str(type, name, len)  syx_request_query_ex((type), 0, (name), (len))

#define SYX_REQUEST_IS_METHOD(x) \
PHP_METHOD(syx_request, is##x) {\
	zval *method = zend_read_property(Z_OBJCE_P(getThis()), \
			getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_METHOD), 0, NULL); \
	if (zend_string_equals_literal_ci(Z_STR_P(method), #x)) { \
		RETURN_TRUE; \
	} \
	RETURN_FALSE; \
}

#define SYX_REQUEST_METHOD(ce, x, type) \
PHP_METHOD(ce, get##x) { \
	zend_string *name; \
	zval *ret; \
	zval *def = NULL; \
	if (ZEND_NUM_ARGS() == 0) { \
		ret = syx_request_query(type, NULL); \
	}else if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def) == FAILURE) { \
		return; \
	} else { \
		ret = syx_request_query(type, name); \
		if (!ret) { \
			if (def != NULL) { \
				RETURN_ZVAL(def, 1, 0); \
			} \
		} \
	} \
	if (ret) { \
	    RETURN_ZVAL(ret, 1, 0); \
	} else { \
		RETURN_NULL(); \
	} \
}


SYX_STARTUP_FUNCTION(request);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
