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
#include "syx_dispatcher.h"
#include "syx_response.h"
#include "syx_exception.h"

#include "responses/syx_response_http.h"
#include "responses/syx_response_swoole.h"
#include "responses/syx_response_swoole_http.h"

zend_class_entry *syx_response_swoole_http_ce;

/** {{{ ARG_INFO
 */
SYX_BEGIN_ARG_INFO_EX(syx_response_swoole_http_construct_arginfo, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, swoole_http_response, Swoole\\Http\\Response, 0)
SYX_END_ARG_INFO()

syx_response_t* syx_response_swoole_http_instance(syx_response_t *this_ptr, syx_response_t *swoole_http_response){
    if(Z_ISUNDEF_P(this_ptr)){
        object_init_ex(this_ptr, syx_response_swoole_http_ce);
        zend_update_property(syx_response_swoole_http_ce, this_ptr, ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_SWOOLE_HTTP_RESPONSE), swoole_http_response);
    }
    return this_ptr;
}

PHP_METHOD(syx_response_swoole_http, __construct) {
    syx_response_t *swoole_http_response;
    if(zend_parse_parameters_throw(ZEND_NUM_ARGS(), "O", &swoole_http_response) == FAILURE){
        return;
    }
    zend_update_property(syx_response_swoole_http_ce, getThis(), ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_SWOOLE_HTTP_RESPONSE), swoole_http_response);
}

PHP_METHOD(syx_response_swoole_http, response) {
    syx_response_t *ztype, *self, *zheaders, *zcookies, *zbody, *zresponse_code, *swoole_http_response, retval = {{0}}, err_code = {{0}};

    zval *val, *entry, *cookie_entry;
    zend_string *header_name, *cookie_name;

    self = getThis();
    swoole_http_response = zend_read_property(syx_response_swoole_http_ce, self, ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_SWOOLE_HTTP_RESPONSE), 0, NULL);

    // header
    zheaders = zend_read_property(syx_response_swoole_http_ce, self, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_HEADER), 1, NULL);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zheaders), header_name, entry) {
        zend_call_method_with_2_params(swoole_http_response, Z_OBJCE_P(swoole_http_response), NULL, "header", &retval, header_name, entry);
    } ZEND_HASH_FOREACH_END();

    // cookie
    zcookies = zend_read_property(syx_response_swoole_http_ce, self, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_COOKIE), 1, NULL);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zcookies), cookie_name, cookie_entry) {
        zend_call_method_with_2_params(swoole_http_response, Z_OBJCE_P(swoole_http_response), NULL, "cookie", &retval, cookie_name, cookie_entry);
    } ZEND_HASH_FOREACH_END();

    // response code
    zresponse_code = zend_read_property(syx_response_swoole_http_ce, self, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_RESPONSECODE), 1, NULL);
    if(zresponse_code == NULL || Z_LVAL_P(zresponse_code) <= 0 ){
        ZVAL_LONG(zresponse_code, 200);
    }else if(Z_TYPE_P(zresponse_code) != IS_LONG){
        syx_trigger_error(SYX_ERR_RESPONSE_ERROR, "response faild, response code must be a int ");
    }
    zend_call_method_with_1_params(swoole_http_response, Z_OBJCE_P(swoole_http_response), NULL, "cookie", &retval, zresponse_code);

    ztype = zend_read_property(syx_response_swoole_http_ce, self, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_SEND_TYPE), 1, NULL);

    switch(Z_LVAL_P(ztype)){
        case SYX_RESPONSE_CONSTANT_NAME_FILE_VALUE:{
            // @TODO send file
            break;
        }
        case SYX_RESPONSE_CONSTANT_NAME_BINARY_VALUE:
        case SYX_RESPONSE_CONSTANT_NAME_TEXT_VALUE:
        case SYX_RESPONSE_CONSTANT_NAME_SIMPLE_VALUE:
        default:{
            zbody = zend_read_property(syx_response_swoole_http_ce, self, ZEND_STRL(SYX_RESPONSE_PROPERTY_NAME_BODY), 1, NULL);
            if(zbody != NULL){
                zend_call_method_with_1_params(swoole_http_response, Z_OBJCE_P(swoole_http_response), NULL, "end", &retval, zbody);
            }else{
                zend_call_method_with_1_params(swoole_http_response, Z_OBJCE_P(swoole_http_response), NULL, "end", &retval, "");
            }
            if(Z_TYPE_P(&retval) == IS_FALSE){
                syx_trigger_error(SYX_ERR_RESPONSE_ERROR, "syx response faild, error");
                zval_ptr_dtor(&retval);
                RETURN_FALSE;
            }
        }
    }
    zval_ptr_dtor(&retval);
    RETURN_TRUE;
}
/* }}} */

PHP_METHOD(syx_response_swoole_http, setGzipLevel){
    syx_response_t *level;
    if(zend_parse_parameters_throw(ZEND_NUM_ARGS(), "l", &level) == FAILURE){
        return;
    }
    zend_update_property_long(syx_response_swoole_http_ce, getThis(), ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_GZIP_LEVEL), level);
    RETURN_TRUE;
}

PHP_METHOD(syx_response_swoole_http, setCookie){
    RETURN_FALSE;
}

PHP_METHOD(syx_response_swoole_http, setAllCookie){
    RETURN_FALSE;
}

zend_function_entry syx_response_swoole_http_methods[] = {
    PHP_ME(syx_response_swoole_http, __construct, syx_response_swoole_http_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(syx_response_swoole_http, response, syx_response_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_response_swoole_http, setCookie, syx_response_http_set_cookie_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_response_swoole_http, setAllCookie, syx_response_http_set_allcookie_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(syx_response_swoole_http, setGzipLevel, syx_response_http_set_gzip_level_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(response_swoole_http) {
    zend_class_entry ce;

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\Response\\Swoole\\Http", syx_response_swoole_http_methods);
    syx_response_swoole_http_ce = zend_register_internal_class_ex(&ce, syx_response_http_ce);
    zend_declare_property_long(syx_response_swoole_http_ce, ZEND_STRL(SYX_RESPONSE_SWOOLE_PROPERTY_NAME_GZIP_LEVEL), 0, ZEND_ACC_PROTECTED);

    return SUCCESS;
}