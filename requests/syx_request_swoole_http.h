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

#ifndef SYX_REQUEST_SERVER_HTTP_H_
#define SYX_REQUEST_SERVER_HTTP_H_

#define SYX_SERVER_REQUEST_NAME_REQUEST "request"
#define SYX_SERVER_REQUEST_NAME_POST "post"
#define SYX_SERVER_REQUEST_NAME_GET "get"
#define SYX_SERVER_REQUEST_NAME_HEADER "header"
#define SYX_SERVER_REQUEST_NAME_SERVER "server"
#define SYX_SERVER_REQUEST_NAME_FILES "files"
#define SYX_SERVER_REQUEST_NAME_COOKIE "cookie"
#define SYX_REQUEST_PROPERTY_NAME_OBJ_HTTP_REQUEST "swoole_http_request"
#define SYX_REQUEST_PROPERTY_NAME_SWOOLE_HTTP_REQUEST_CE "swoole_http_request_ce"

#define SYX_SERVER_REQUEST_VARS_TYPE                 unsigned int
#define SYX_SERVER_REQUEST_VARS_POST                 TRACK_VARS_POST
#define SYX_SERVER_REQUEST_VARS_GET                  TRACK_VARS_GET
#define SYX_SERVER_REQUEST_VARS_ENV                  TRACK_VARS_ENV
#define SYX_SERVER_REQUEST_VARS_FILES                TRACK_VARS_FILES
#define SYX_SERVER_REQUEST_VARS_SERVER               TRACK_VARS_SERVER
#define SYX_SERVER_REQUEST_VARS_REQUEST              TRACK_VARS_REQUEST
#define SYX_SERVER_REQUEST_VARS_COOKIE               TRACK_VARS_COOKIE

syx_request_t* syx_request_swoole_http_instance(syx_request_t *this_ptr, syx_request_t *swoole_http_request);
syx_request_t* syx_request_swoole_http_query(syx_request_t *this_ptr, const char *type, size_t type_len, zend_string *key);

extern zend_class_entry* syx_request_swoole_http_ce;

SYX_STARTUP_FUNCTION(request_swoole_http);

#endif /* SYX_REQUEST_SERVER_HTTP_H_ */
