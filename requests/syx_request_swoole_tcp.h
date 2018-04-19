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

#ifndef SYX_REQUEST_SWOOLE_TCP_H_
#define SYX_REQUEST_SWOOLE_TCP_H_

#define SYX_REQUEST_PROPERTY_NAME_RAW "_raw"
#define SYX_REQUEST_PROPERTY_NAME_FD "_fd"
#define SYX_REQUEST_PROPERTY_NAME_REACTOR_ID "_reactor_id"
#define SYX_REQUEST_PROPERTY_NAME_DATA "_data"

extern zend_class_entry* syx_request_swoole_tcp_ce;
syx_request_t *syx_request_swoole_tcp_instance(syx_request_t *this_ptr, syx_request_t *fd, syx_request_t *reactor_id, syx_request_t *data);

SYX_STARTUP_FUNCTION(request_swoole_tcp);

#endif /* SYX_REQUEST_SWOOLE_TCP_H_ */
