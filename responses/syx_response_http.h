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


#ifndef PHP_SYX_RESPONSE_HTTP_H
#define PHP_SYX_RESPONSE_HTTP_H

#define SYX_RESPONSE_PROPERTY_NAME_RESPONSECODE     "_response_code"
#define SYX_RESPONSE_PROPERTY_NAME_COOKIE           "_cookie"
#define SYX_RESPONSE_PROPERTY_NAME_HEADER           "_header"

SYX_BEGIN_ARG_INFO_EX(syx_response_http_set_header_arginfo, 0, 0, 2)
  SYX_ARG_INFO(0, name)
  SYX_ARG_INFO(0, value)
  SYX_ARG_INFO(0, rep)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_http_get_header_arginfo, 0, 0, 0)
  SYX_ARG_INFO(0, name)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_http_set_all_headers_arginfo, 0, 0, 1)
  SYX_ARG_INFO(0, headers)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_http_set_redirect_arginfo, 0, 0, 1)
  SYX_ARG_INFO(0, url)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_http_set_cookie_arginfo, 0, 0, 2)
    SYX_ARG_INFO(0, fd)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_http_set_allcookie_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, key)
    SYX_ARG_INFO(0, value)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_http_set_response_code_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, code)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_http_response_file_arginfo, 0, 0, 2)
    SYX_ARG_INFO(0, mime_type)
    SYX_ARG_INFO(0, file_path)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_response_http_set_gzip_level_arginfo, 0, 0, 1)
    SYX_ARG_INFO(0, level)
SYX_END_ARG_INFO()

extern zend_class_entry *syx_response_http_ce;

int syx_response_alter_header(syx_response_t *response, zend_string *name, char *value, long value_len, uint rep);
zval * syx_response_get_header(syx_response_t *response, zend_string *name);
int syx_response_clear_header(syx_response_t *response, zend_string *name);

SYX_STARTUP_FUNCTION(response_http);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
