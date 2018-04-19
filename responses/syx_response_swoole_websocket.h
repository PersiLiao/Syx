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

#ifndef SYX_SERVER_RESPONSE_WEBSOCKET_H_
#define SYX_SERVER_RESPONSE_WEBSOCKET_H_

extern zend_class_entry *syx_response_swoole_websocket_ce;

syx_response_t* syx_response_swoole_websocket_instance(syx_response_t *this_ptr, syx_response_t *swoole_websocket_frame);

SYX_STARTUP_FUNCTION(syx_response_swoole_websocket);

#endif /* SYX_SERVER_RESPONSE_WEBSOCKET_H_ */
