/*
  +----------------------------------------------------------------------+
  | Syx Framework                                                        |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: XiangChu Liao  <ican.lxc@gmail.com>                          |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "main/SAPI.h" /* for sapi_header_line */
#include "Zend/zend_interfaces.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_config.h"
#include "syx_loader.h"
#include "syx_application.h"
#include "syx_bootstrap.h"
#include "syx_dispatcher.h"
#include "syx_exception.h"

#include "syx_request.h"
#include "syx_server.h"
#include "syx_request_http.h"
#include "syx_request_swoole_http.h"

zend_class_entry* syx_request_swoole_http_ce;

SYX_BEGIN_ARG_INFO_EX(syx_request_swoole_http_construct_arginfo, 0, 0, 1)
    SYX_ARG_OBJ_INFO(0, swoole_http_request, Swoole\\Http\\Request, 0)
SYX_END_ARG_INFO()

syx_request_t *syx_request_swoole_http_instance(syx_request_t *this_ptr, syx_request_t *swoole_http_request){
    zval params, *request_http_server = NULL, *request_uri, *request_method;
    zend_string *settled_uri = NULL;
    HashTable *request_http;

    if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, syx_request_swoole_http_ce);
    }

    request_http_server = zend_read_property(Z_OBJCE_P(swoole_http_request), swoole_http_request, ZEND_STRL("server"), 0, NULL);
    request_method = zend_hash_str_find(Z_ARRVAL_P(request_http_server), ZEND_STRL("request_method"));
    if(request_method == NULL){
        syx_trigger_error(SYX_ERR_STARTUP_FAILED, "server request_method of request failed");
        return NULL;
    }
    zend_update_property(syx_request_swoole_http_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_METHOD), request_method);
    request_uri = zend_hash_str_find(Z_ARRVAL_P(request_http_server), ZEND_STRL("request_uri"));
    zend_update_property(syx_request_swoole_http_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_URI), request_uri );
    zend_update_property(syx_request_swoole_http_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_OBJ_HTTP_REQUEST), swoole_http_request );

    settled_uri = zend_string_init(Z_STRVAL_P(request_uri), Z_STRLEN_P(request_uri), 0);
    syx_request_set_base_uri(this_ptr, SYX_G(base_uri), settled_uri);
    zend_string_release(settled_uri);
    syx_request_params_init(this_ptr);
    return this_ptr;
}

syx_request_t* syx_request_swoole_http_query(syx_request_t* this_ptr, const char* type, size_t type_len, zend_string* key){
    syx_request_t *data, *vararr = NULL, *swoole_http_request_o;

    swoole_http_request_o = zend_read_property(syx_request_swoole_http_ce, this_ptr, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_OBJ_HTTP_REQUEST), 0, NULL);
    vararr = zend_read_property(Z_OBJCE_P(swoole_http_request_o), swoole_http_request_o, type, type_len, 0, NULL);
    if(vararr == NULL || !vararr || Z_TYPE_P(vararr) != IS_ARRAY){
        return NULL;
    }
    data = zend_hash_find(Z_ARRVAL_P(vararr), key);
    if(!data){
        return NULL;
    }
    return data;
}

PHP_METHOD(syx_request_swoole_http, getQuery){
    syx_request_t *data, *def = NULL, *ret;
    zend_string *key;
    if(zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &key, &def) == FAILURE){
        return;
    }
    ret = syx_request_swoole_http_query(getThis(), ZEND_STRL(SYX_SERVER_REQUEST_NAME_GET), key);
    if (ret) {
        RETURN_ZVAL(ret, 1, 0);
    } else {
        if(def != NULL){
            RETURN_ZVAL(def, 1, 0);
        }else{
            RETURN_NULL();
        }
    }
}

PHP_METHOD(syx_request_swoole_http, getRequest){

}

PHP_METHOD(syx_request_swoole_http, getPost){
    syx_request_t *data, *def = NULL, *ret;
    zend_string *key;
    if(zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &key, &def) == FAILURE){
        return;
    }
    ret = syx_request_swoole_http_query(getThis(), ZEND_STRL(SYX_SERVER_REQUEST_NAME_POST), key);
    if (ret) {
        RETURN_ZVAL(ret, 1, 0);
    } else {
        if(def != NULL){
            RETURN_ZVAL(def, 1, 0);
        }else{
            RETURN_NULL();
        }
    }
}

PHP_METHOD(syx_request_swoole_http, getCookie){
    syx_request_t *data, *def = NULL, *ret;
    zend_string *key;
    if(zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &key, &def) == FAILURE){
        return;
    }
    ret = syx_request_swoole_http_query(getThis(), ZEND_STRL(SYX_SERVER_REQUEST_NAME_COOKIE), key);
    if (ret) {
        RETURN_ZVAL(ret, 1, 0);
    } else {
        if(def != NULL){
            RETURN_ZVAL(def, 1, 0);
        }else{
            RETURN_NULL();
        }
    }
}

PHP_METHOD(syx_request_swoole_http, getFiles){
    syx_request_t *data, *def = NULL, *ret;
    zend_string *key;
    if(zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &key, &def) == FAILURE){
        return;
    }
    ret = syx_request_swoole_http_query(getThis(), ZEND_STRL(SYX_SERVER_REQUEST_NAME_FILES), key);
    if (ret) {
        RETURN_ZVAL(ret, 1, 0);
    } else {
        if(def != NULL){
            RETURN_ZVAL(def, 1, 0);
        }else{
            RETURN_NULL();
        }
    }
}

PHP_METHOD(syx_request_swoole_http, getServer){
    syx_request_t *data, *def = NULL, *ret;
    zend_string *key;
    if(zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &key, &def) == FAILURE){
        return;
    }
    ret = syx_request_swoole_http_query(getThis(), ZEND_STRL(SYX_SERVER_REQUEST_NAME_SERVER), key);
    if (ret) {
        RETURN_ZVAL(ret, 1, 0);
    } else {
        if(def != NULL){
            RETURN_ZVAL(def, 1, 0);
        }else{
            RETURN_NULL();
        }
    }
}

PHP_METHOD(syx_request_swoole_http, get){
    RETURN_NULL();
}

PHP_METHOD(syx_request_swoole_http, isXmlHttpRequest) {
    syx_request_t *data, *ret;
    zend_string *key;
    key = zend_string_init("HTTP_X_REQUESTED_WITH", sizeof("HTTP_X_REQUESTED_WITH") - 1, 0);
    ret = syx_request_swoole_http_query(getThis(), ZEND_STRL(SYX_SERVER_REQUEST_NAME_GET), key);
    if (ret && Z_TYPE_P(ret) == IS_STRING
            && strncasecmp("XMLHttpRequest", Z_STRVAL_P(ret), Z_STRLEN_P(ret)) == 0) {
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

PHP_METHOD(syx_request_swoole_http, __construct){
    syx_request_t *self = getThis(), *sw_http_request;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "O", &sw_http_request) == FAILURE) {
        return;
    }

    (void)syx_request_swoole_http_instance(self, sw_http_request);
}

PHP_METHOD(syx_request_swoole_http, getLanguage){
    RETURN_NULL();
}

PHP_METHOD(syx_request_swoole_http, getEnv){
    RETURN_NULL();
}

PHP_METHOD(syx_request_swoole_http, getRaw){
    syx_request_t *sw_http_request, syx_request_raw = {{0}};

    sw_http_request = zend_read_property(syx_request_swoole_http_ce, getThis(), ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_OBJ_HTTP_REQUEST), 0, NULL);
    if(sw_http_request == NULL || Z_TYPE_P(sw_http_request) != IS_OBJECT){
        RETURN_NULL();
    }
    zend_call_method_with_0_params(sw_http_request, Z_OBJCE_P(sw_http_request), NULL, "getRawContent", &syx_request_raw);
    RETURN_ZVAL(&syx_request_raw, 1, 1);
}

zend_function_entry syx_request_swoole_http_methods[] = {
    PHP_ME(syx_request_swoole_http, getLanguage, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, getServer, syx_request_http_getserver_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, getEnv, syx_request_http_getenv_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, getRaw, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, getQuery, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, getRequest, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, getPost, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, getCookie, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, getFiles, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, get, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, isXmlHttpRequest, syx_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_request_swoole_http, __construct, syx_request_swoole_http_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(request_swoole_http){
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\Request\\Swoole\\Http", syx_request_swoole_http_methods);
    syx_request_swoole_http_ce = zend_register_internal_class_ex(&ce, syx_request_http_ce);
    zend_declare_property_null(syx_request_swoole_http_ce, ZEND_STRL(SYX_REQUEST_PROPERTY_NAME_OBJ_HTTP_REQUEST), ZEND_ACC_PROTECTED);

    return SUCCESS;
}