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
#include "main/SAPI.h"
#include "ext/standard/url.h" /* for php_url */

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_request.h"
#include "syx_exception.h"

#include "requests/syx_request_http.h"

static zend_class_entry * syx_request_http_ce;

syx_request_t *syx_request_http_instance(syx_request_t *this_ptr, zend_string *request_uri, zend_string *base_uri) /* {{{ */ {
	zval method, params;
	zend_string *settled_uri = NULL;

	if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, syx_request_http_ce);
	}

	if (SG(request_info).request_method) {
		ZVAL_STRING(&method, (char *)SG(request_info).request_method);
	} else if (strncasecmp(sapi_module.name, "cli", 3)) {
		ZVAL_STRING(&method, "Unknow");
	} else {
		ZVAL_STRING(&method, "Cli");
	}

	zend_update_property(syx_request_http_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_METHOD), &method);
	zval_ptr_dtor(&method);

	if (request_uri) {
		settled_uri = zend_string_copy(request_uri);
	} else {
		zval *uri;
		do {
#ifdef PHP_WIN32
			zval *rewrited;
			/* check this first so IIS will catch */
			uri = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "HTTP_X_REWRITE_URL", sizeof("HTTP_X_REWRITE_URL") - 1);
			if (uri) {
				if (EXPECTED(Z_TYPE_P(uri) == IS_STRING))  {
					settled_uri = zend_string_copy(Z_STR_P(uri));
					break;
				}
			}

			/* IIS7 with URL Rewrite: make sure we get the unencoded url (double slash problem) */
			rewrited = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "IIS_WasUrlRewritten", sizeof("IIS_WasUrlRewritten") - 1);
			if (rewrited) {
				if (zend_is_true(rewrited)) {
					uri = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "UNENCODED_URL", sizeof("UNENCODED_URL") - 1);
					if (uri) {
						if (EXPECTED(Z_TYPE_P(uri) == IS_STRING && Z_STRLEN_P(uri))) {
							settled_uri = zend_string_copy(Z_STR_P(uri));
							break;
						}
					}
				}
			}
#endif
			uri = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "PATH_INFO", sizeof("PATH_INFO") - 1);
			if (uri) {
				if (EXPECTED(Z_TYPE_P(uri) == IS_STRING)) {
					settled_uri = zend_string_copy(Z_STR_P(uri));
					break;
				}
			}

			uri = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "REQUEST_URI", sizeof("REQUEST_URI") - 1);
			if (uri) {
				if (EXPECTED(Z_TYPE_P(uri) == IS_STRING)) {
					/* Http proxy reqs setup request uri with scheme and host [and port] + the url path,
					 * only use url path */
					if (strncasecmp(Z_STRVAL_P(uri), "http", sizeof("http") - 1) == 0) {
						php_url *url_info = php_url_parse(Z_STRVAL_P(uri));
						if (url_info && url_info->path) {
							settled_uri = zend_string_init(url_info->path, strlen(url_info->path), 0);
						}
						php_url_free(url_info);
					} else {
						char *pos = NULL;
						if ((pos = strstr(Z_STRVAL_P(uri), "?"))) {
							settled_uri = zend_string_init(Z_STRVAL_P(uri), pos - Z_STRVAL_P(uri), 0);
						} else {
							settled_uri = zend_string_copy(Z_STR_P(uri));
						}
					}
					break;
				}
			}

			uri = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "ORIG_PATH_INFO", sizeof("ORIG_PATH_INFO") - 1);
			if (uri) {
				if (EXPECTED(Z_TYPE_P(uri) == IS_STRING)) {
					settled_uri = zend_string_copy(Z_STR_P(uri));
					break;
				}
			}
		} while (0);
	}

	if (settled_uri) {
		char *p = ZSTR_VAL(settled_uri);

		while (*p == '/' && *(p + 1) == '/') {
			p++;
		}

		if (p != ZSTR_VAL(settled_uri)) {
			zend_string *garbage = settled_uri;
			settled_uri = zend_string_init(p, ZSTR_LEN(settled_uri) - (p - ZSTR_VAL(settled_uri)), 0);
			zend_string_release(garbage);
		}

		zend_update_property_str(syx_request_http_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_URI), settled_uri);
		syx_request_set_base_uri(this_ptr, base_uri, settled_uri);
		zend_string_release(settled_uri);
	}

	array_init(&params);
	zend_update_property(syx_request_http_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_PARAMS), &params);
	zval_ptr_dtor(&params);

	return this_ptr;
}
/* }}} */

/** {{{ proto public Syx_Request_Http::getQuery(mixed $name, mixed $default = NULL)
*/
SYX_REQUEST_METHOD(syx_request_http, Query, 	SYX_GLOBAL_VARS_GET);
/* }}} */

/** {{{ proto public Syx_Request_Http::getPost(mixed $name, mixed $default = NULL)
*/
SYX_REQUEST_METHOD(syx_request_http, Post,  	SYX_GLOBAL_VARS_POST);
/* }}} */

/** {{{ proto public Syx_Request_Http::getRequet(mixed $name, mixed $default = NULL)
*/
SYX_REQUEST_METHOD(syx_request_http, Request, SYX_GLOBAL_VARS_REQUEST);
/* }}} */

/** {{{ proto public Syx_Request_Http::getFiles(mixed $name, mixed $default = NULL)
*/
SYX_REQUEST_METHOD(syx_request_http, Files, 	SYX_GLOBAL_VARS_FILES);
/* }}} */

/** {{{ proto public Syx_Request_Http::getCookie(mixed $name, mixed $default = NULL)
*/
SYX_REQUEST_METHOD(syx_request_http, Cookie, 	SYX_GLOBAL_VARS_COOKIE);
/* }}} */

/** {{{ proto public Syx_Request_Http::isXmlHttpRequest()
*/
PHP_METHOD(syx_request_http, isXmlHttpRequest) {
	zend_string *name;
	zval * header;
	name = zend_string_init("HTTP_X_REQUESTED_WITH", sizeof("HTTP_X_REQUESTED_WITH") - 1, 0);
	header = syx_request_query(SYX_GLOBAL_VARS_SERVER, name);
	zend_string_release(name);
	if (header && Z_TYPE_P(header) == IS_STRING
			&& strncasecmp("XMLHttpRequest", Z_STRVAL_P(header), Z_STRLEN_P(header)) == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Request_Http::get(mixed $name, mixed $default)
 * params -> post -> get -> cookie -> server
 */
PHP_METHOD(syx_request_http, get) {
	zend_string	*name 	= NULL;
	zval 	*def 	= NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		zval *value = syx_request_get_param(getThis(), name);
		if (value) {
			RETURN_ZVAL(value, 1, 0);
		} else {
			zval *params	= NULL;
			zval *pzval	= NULL;

			SYX_GLOBAL_VARS_TYPE methods[4] = {
				SYX_GLOBAL_VARS_POST,
				SYX_GLOBAL_VARS_GET,
				SYX_GLOBAL_VARS_COOKIE,
				SYX_GLOBAL_VARS_SERVER
			};

			{
				int i = 0;
				for (;i<4; i++) {
					params = &PG(http_globals)[methods[i]];
					if (params && Z_TYPE_P(params) == IS_ARRAY) {
						if ((pzval = zend_hash_find(Z_ARRVAL_P(params), name)) != NULL ){
							RETURN_ZVAL(pzval, 1, 0);
						}
					}
				}

			}
			if (def) {
				RETURN_ZVAL(def, 1, 0);
			}
		}
	}
	RETURN_NULL();
}
/* }}} */

/** {{{ proto public Syx_Request_Http::__construct(string $request_uri, string $base_uri)
*/
PHP_METHOD(syx_request_http, __construct) {
	zend_string *request_uri = NULL;
	zend_string *base_uri = NULL;
	syx_request_t *self = getThis();

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|SS", &request_uri, &base_uri) == FAILURE) {
		return;
	}

	(void)syx_request_http_instance(self, request_uri, base_uri);
}
/* }}} */

/** {{{ proto private Syx_Request_Http::__clone
 */
PHP_METHOD(syx_request_http, __clone) {
}
/* }}} */

/** {{{ syx_request_http_methods
 */
zend_function_entry syx_request_http_methods[] = {
	PHP_ME(syx_request_http, getQuery, 		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request_http, getRequest, 		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request_http, getPost, 		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request_http, getCookie,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request_http, getFiles,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request_http, get,			NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request_http, isXmlHttpRequest, 	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(syx_request_http, __construct,		NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(syx_request_http, __clone,		NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CLONE)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
 */
SYX_STARTUP_FUNCTION(request_http){
	zend_class_entry ce;
	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Request\\Http", syx_request_http_methods);
	syx_request_http_ce = zend_register_internal_class_ex(&ce, syx_request_ce);

	zend_declare_class_constant_string(syx_request_http_ce, ZEND_STRL("SCHEME_HTTP"), "http");
	zend_declare_class_constant_string(syx_request_http_ce, ZEND_STRL("SCHEME_HTTPS"), "https");

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
