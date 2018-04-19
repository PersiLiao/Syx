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

#ifndef SYX_SERVER_RESPONSE_H_
#define SYX_SERVER_RESPONSE_H_

#define SYX_RESPONSE_SWOOLE_PROPERTY_NAME_SWOOLE_HTTP_RESPONSE "_swoole_http_response"
#define SYX_RESPONSE_SWOOLE_PROPERTY_NAME_GZIP_LEVEL "_gzip_level"

extern zend_class_entry *syx_response_swoole_http_ce;

syx_response_t* syx_response_swoole_http_instance(syx_response_t *this_ptr, syx_response_t *swoole_http_response);

SYX_STARTUP_FUNCTION(response_swoole_http);

#endif /* SYX_SERVER_RESPONSE_H_ */
