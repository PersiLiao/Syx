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

#ifndef PHP_SYX_RESPONSE_H
#define PHP_SYX_RESPONSE_H

#define SYX_RESPONSE_PROPERTY_NAME_BODY				"_body"
#define SYX_RESPONSE_PROPERTY_NAME_DEFAULTBODY		"content"
#define SYX_RESPONSE_PROPERTY_NAME_DEFAULTBODYNAME  "DEFAULT_BODY"
#define SYX_RESPONSE_PROPERTY_NAME_SEND_TYPE "_send_type"

#define SYX_RESPONSE_CONSTANT_NAME_SIMPLE  "SIMPLE"
#define SYX_RESPONSE_CONSTANT_NAME_SIMPLE_VALUE 0
#define SYX_RESPONSE_CONSTANT_NAME_TEXT    "TEXT"
#define SYX_RESPONSE_CONSTANT_NAME_TEXT_VALUE 1
#define SYX_RESPONSE_CONSTANT_NAME_FILE    "FILE"
#define SYX_RESPONSE_CONSTANT_NAME_FILE_VALUE 2
#define SYX_RESPONSE_CONSTANT_NAME_BINARY  "BINARY"
#define SYX_RESPONSE_CONSTANT_NAME_BINARY_VALUE 3

#define SYX_RESPONSE_REPLACE 0
#define SYX_RESPONSE_PREPEND 1
#define SYX_RESPONSE_APPEND  2

SYX_BEGIN_ARG_INFO_EX(syx_response_void_arginfo, 0, 0, 0)
SYX_END_ARG_INFO()

extern zend_class_entry *syx_response_ce;

syx_response_t * syx_response_instance(syx_dispatcher_t *syx_dispatcher, syx_response_t *this_ptr);
int syx_response_alter_body(syx_response_t *response, zend_string *name, zend_string *body, int flag);
int syx_response_set_redirect(syx_response_t *response, char *url, int len);
int syx_response_clear_body(syx_response_t *response, zend_string *name);

SYX_STARTUP_FUNCTION(response);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
