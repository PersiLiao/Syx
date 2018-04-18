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
#include "standard/php_string.h" /* for php_basename */
#include "ext/standard/url.h" /* for php_url */
#include "Zend/zend_smart_str.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_request.h"
#include "syx_exception.h"

#include "requests/syx_request_http.h"

zend_class_entry *syx_request_http_ce;

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

syx_request_t *syx_request_instance(syx_request_t *this_ptr, zend_string *request_uri) /* {{{ */ {
    return syx_request_http_instance(this_ptr, NULL, request_uri);
}
/* }}} */

int syx_request_set_base_uri(syx_request_t *request, zend_string *base_uri, zend_string *request_uri) /* {{{ */ {
    if (base_uri == NULL) {
        zend_string *basename = NULL;
        zval *script_filename = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME") - 1);
        do {
            if (script_filename && IS_STRING == Z_TYPE_P(script_filename)) {
                zend_string *file_name;
                char *ext = ZSTR_VAL(SYX_G(ext));
                size_t ext_len = ZSTR_LEN(SYX_G(ext));
                zval *script_name, *phpself_name, *orig_name;

                script_name = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "SCRIPT_NAME", sizeof("SCRIPT_NAME") - 1);
                file_name = php_basename(Z_STRVAL_P(script_filename), Z_STRLEN_P(script_filename), ext, ext_len);
                if (script_name && EXPECTED(IS_STRING == Z_TYPE_P(script_name))) {
                    zend_string *script = php_basename(Z_STRVAL_P(script_name), Z_STRLEN_P(script_name), NULL, 0);

                    if (strncmp(ZSTR_VAL(file_name), ZSTR_VAL(script), ZSTR_LEN(file_name)) == 0) {
                        basename = zend_string_copy(Z_STR_P(script_name));
                        zend_string_release(file_name);
                        zend_string_release(script);
                        break;
                    }
                    zend_string_release(script);
                }

                phpself_name = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "PHP_SELF", sizeof("PHP_SELF") - 1);
                if (phpself_name && EXPECTED(IS_STRING == Z_TYPE_P(phpself_name))) {
                    zend_string *phpself = php_basename(Z_STRVAL_P(phpself_name), Z_STRLEN_P(phpself_name), NULL, 0);
                    if (strncmp(ZSTR_VAL(file_name), ZSTR_VAL(phpself), ZSTR_LEN(file_name)) == 0) {
                        basename = zend_string_copy(Z_STR_P(phpself_name));
                        zend_string_release(file_name);
                        zend_string_release(phpself);
                        break;
                    }
                    zend_string_release(phpself);
                }

                orig_name = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "ORIG_SCRIPT_NAME", sizeof("ORIG_SCRIPT_NAME") - 1);
                if (orig_name && IS_STRING == Z_TYPE_P(orig_name)) {
                    zend_string *orig = php_basename(Z_STRVAL_P(orig_name), Z_STRLEN_P(orig_name), NULL, 0);
                    if (strncmp(ZSTR_VAL(file_name), ZSTR_VAL(orig), ZSTR_LEN(file_name)) == 0) {
                        basename = zend_string_copy(Z_STR_P(orig_name));
                        zend_string_release(file_name);
                        zend_string_release(orig);
                        break;
                    }
                    zend_string_release(orig);
                }
                zend_string_release(file_name);
            }
        } while (0);

        if (basename && strncmp(ZSTR_VAL(request_uri), ZSTR_VAL(basename), ZSTR_LEN(basename)) == 0) {
            if (*(ZSTR_VAL(basename) + ZSTR_LEN(basename) - 1) == '/') {
                zend_string *garbage = basename;
                basename = zend_string_init(ZSTR_VAL(basename), ZSTR_LEN(basename) - 1, 0);
                zend_string_release(garbage);
            }
            zend_update_property_str(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_BASE), basename);
            zend_string_release(basename);
            return 1;
        } else if (basename) {
            zend_string *dir = zend_string_init(ZSTR_VAL(basename), ZSTR_LEN(basename), 0); /* php_dirname might alter the string */

            ZSTR_LEN(dir) = php_dirname(ZSTR_VAL(dir), ZSTR_LEN(dir));
            if (*(ZSTR_VAL(dir) + ZSTR_LEN(dir) - 1) == '/') {
                --ZSTR_LEN(dir);
            }

            if (ZSTR_LEN(dir)) {
                if (strncmp(ZSTR_VAL(request_uri), ZSTR_VAL(dir), ZSTR_LEN(dir)) == 0) {
                    zend_update_property_str(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_BASE), dir);
                    zend_string_release(dir);
                    zend_string_release(basename);
                    return 1;
                }
            }
            zend_string_release(dir);
            zend_string_release(basename);
        }

        zend_update_property_string(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_BASE), "");
    } else {
        zend_update_property_str(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_BASE), base_uri);
    }
    return 1;
}
/* }}} */

zval *syx_request_query_ex(uint type, zend_bool fetch_type, void *name, size_t len) /* {{{ */ {
    zval *carrier = NULL, *ret;

    zend_bool jit_initialization = PG(auto_globals_jit);

    /* for phpunit test requirements */
#if PHP_SYX_DEBUG
    switch (type) {
        case SYX_GLOBAL_VARS_POST:
            carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_POST"));
            break;
        case SYX_GLOBAL_VARS_GET:
            carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_GET"));
            break;
        case SYX_GLOBAL_VARS_COOKIE:
            carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_COOKIE"));
            break;
        case SYX_GLOBAL_VARS_SERVER:
            if (jit_initialization) {
                zend_string *server_str = zend_string_init("_SERVER", sizeof("_SERVER") - 1, 0);
                zend_is_auto_global(server_str);
                zend_string_release(server_str);
            }
            carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_SERVER"));
            break;
        case SYX_GLOBAL_VARS_ENV:
            if (jit_initialization) {
                zend_string *env_str = zend_string_init("_ENV", sizeof("_ENV") - 1, 0);
                zend_is_auto_global(env_str);
                zend_string_release(env_str);
            }
            carrier = &PG(http_globals)[SYX_GLOBAL_VARS_ENV];
            break;
        case SYX_GLOBAL_VARS_FILES:
            carrier = &PG(http_globals)[SYX_GLOBAL_VARS_FILES];
            break;
        case SYX_GLOBAL_VARS_REQUEST:
            if (jit_initialization) {
                zend_string *request_str = zend_string_init("_REQUEST", sizeof("_REQUEST") - 1, 0);
                zend_is_auto_global(request_str);
                zend_string_release(request_str);
            }
            carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_REQUEST"));
            break;
        default:
            break;
    }
#else
    switch (type) {
        case SYX_GLOBAL_VARS_POST:
        case SYX_GLOBAL_VARS_GET:
        case SYX_GLOBAL_VARS_FILES:
        case SYX_GLOBAL_VARS_COOKIE:
            carrier = &PG(http_globals)[type];
            break;
        case SYX_GLOBAL_VARS_ENV:
            if (jit_initialization) {
                zend_string *env_str = zend_string_init("_ENV", sizeof("_ENV") - 1, 0);
                zend_is_auto_global(env_str);
                zend_string_release(env_str);
            }
            carrier = &PG(http_globals)[type];
            break;
        case SYX_GLOBAL_VARS_SERVER:
            if (jit_initialization) {
                zend_string *server_str = zend_string_init("_SERVER", sizeof("_SERVER") - 1, 0);
                zend_is_auto_global(server_str);
                zend_string_release(server_str);
            }
            carrier = &PG(http_globals)[type];
            break;
        case SYX_GLOBAL_VARS_REQUEST:
            if (jit_initialization) {
                zend_string *request_str = zend_string_init("_REQUEST", sizeof("_REQUEST") - 1, 0);
                zend_is_auto_global(request_str);
                zend_string_release(request_str);
            }
            carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_REQUEST"));
            break;
        default:
            break;
    }
#endif

    if (!carrier) {
        return NULL;
    }

    if (!name) {
        return carrier;
    }

    if (EXPECTED(fetch_type)) {
        if ((ret = zend_hash_find(Z_ARRVAL_P(carrier), (zend_string *)name)) == NULL) {
            return NULL;
        }
    } else {
        if ((ret = zend_hash_str_find(Z_ARRVAL_P(carrier), (char *)name, len)) == NULL) {
            return NULL;
        }
    }
    return ret;
}
/* }}} */

syx_request_t * syx_request_get_method(syx_request_t *request) /* {{{ */ {
    return zend_read_property(syx_request_ce, request, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_METHOD), 1, NULL);
}
/* }}} */

zval *syx_request_get_language(syx_request_t *instance, zval *accept_language) /* {{{ */ {
    zval *lang = zend_read_property(syx_request_ce,
            instance, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_LANG), 1, NULL);

    if (lang && UNEXPECTED(IS_STRING != Z_TYPE_P(lang))) {
        zval *accept_langs = syx_request_query_str(SYX_GLOBAL_VARS_SERVER,
                "HTTP_ACCEPT_LANGUAGE", sizeof("HTTP_ACCEPT_LANGUAGE") - 1);
        if (!accept_langs) {
            return NULL;
        } else if (UNEXPECTED(IS_STRING != Z_TYPE_P(accept_langs) || !Z_STRLEN_P(accept_langs))) {
            return NULL;
        } else {
            char    *ptrptr, *seg;
            uint    prefer_len = 0;
            double  max_qvlaue = 0;
            char    *prefer = NULL;
            char    *langs = estrndup(Z_STRVAL_P(accept_langs), Z_STRLEN_P(accept_langs));

            seg = php_strtok_r(langs, ",", &ptrptr);
            while(seg) {
                char *qvalue;
                while (*(seg) == ' ') {
                    seg++;
                }
                /* Accept-Language: da, en-gb;q=0.8, en;q=0.7 */
                if ((qvalue = strstr(seg, "q="))) {
                    float qval = strtod(qvalue + 2, NULL);
                    if (qval > max_qvlaue) {
                        max_qvlaue = qval;
                        if (prefer) {
                            efree(prefer);
                        }
                        prefer_len = qvalue - seg - 1;
                        prefer     = estrndup(seg, prefer_len);
                    }
                } else {
                    if (max_qvlaue < 1) {
                        max_qvlaue = 1;
                        prefer_len = strlen(seg);
                        prefer     = estrndup(seg, prefer_len);
                    }
                }

                seg = php_strtok_r(NULL, ",", &ptrptr);
            }

            if (prefer) {
                ZVAL_STRINGL(accept_language, prefer, prefer_len);
                zend_update_property(syx_request_ce,
                        instance, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_LANG), accept_language);
                efree(prefer);
                efree(langs);
                zval_ptr_dtor(accept_language);
                return accept_language;
            }
            efree(langs);
            zval_ptr_dtor(accept_language);
        }
    }
    return lang;
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

/** {{{ proto public Syx_Request_Abstract::getEnv(mixed $name, mixed $default = NULL)
*/
SYX_REQUEST_METHOD(syx_request_http, Env,    SYX_GLOBAL_VARS_ENV);
/* }}} */

/** {{{ proto public Syx_Request_Abstract::getServer(mixed $name, mixed $default = NULL)
*/
SYX_REQUEST_METHOD(syx_request_http, Server, SYX_GLOBAL_VARS_SERVER);
/* }}} */

/** {{{ proto public Syx_Request_Abstract::isGet(void)
*/
SYX_REQUEST_IS_METHOD(Get);
/* }}} */

/** {{{ proto public Syx_Request_Abstract::isPost(void)
*/
SYX_REQUEST_IS_METHOD(Post);
/* }}} */

/** {{{ proto public Syx_Request_Abstract::isPut(void)
*/
SYX_REQUEST_IS_METHOD(Put);
/* }}} */

/** {{{ proto public Syx_Request_Abstract::isHead(void)
*/
SYX_REQUEST_IS_METHOD(Head);
/* }}} */

/** {{{ proto public Syx_Request_Abstract::isOptions(void)
*/
SYX_REQUEST_IS_METHOD(Options);
/* }}} */

/** {{{ proto public Syx_Request_Abstract::isCli(void)
*/
SYX_REQUEST_IS_METHOD(Cli);
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

/** {{{ proto public Syx_Request_Abstract::getLanguage(void)
*/
PHP_METHOD(syx_request_http, getLanguage) {
    zval accept_language;
    zval *lang = syx_request_get_language(getThis(), &accept_language);
    if (lang) {
        RETURN_ZVAL(lang, 1, 0);
    } else {
        RETURN_NULL();
    }
}
/* }}} */

PHP_METHOD(syx_request_http, getRaw) {
    php_stream *s;
    smart_str raw_data = {0};

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    s = SG(request_info).request_body;
    if (!s || FAILURE == php_stream_rewind(s)) {
        RETURN_FALSE;
    }

    while (!php_stream_eof(s)) {
        char buf[512];
        size_t len = php_stream_read(s, buf, sizeof(buf));

        if (len && len != (size_t) -1) {
           smart_str_appendl(&raw_data, buf, len);
        }
    }

    if (raw_data.s) {
        smart_str_0(&raw_data);
        RETURN_STR(raw_data.s);
    } else {
        RETURN_FALSE;
    }
}

/** {{{ syx_request_http_methods
 */
zend_function_entry syx_request_http_methods[] = {
    PHP_ME(syx_request_http, isGet, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, isPost, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, isPut, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, isHead, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, isOptions, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, isCli, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, isXmlHttpRequest, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, getLanguage, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, getServer, syx_request_http_getserver_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, getEnv, syx_request_http_getenv_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, getRaw, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, getQuery, 		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, getRequest, 		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, getPost, 		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, getCookie,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, getFiles,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, get,			NULL, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_http, __construct,		NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
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
