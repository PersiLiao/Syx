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

#ifndef SYX_SERVER_WEBSOCKET_H_
#define SYX_SERVER_WEBSOCKET_H_

#define SYX_SERVER_WEBSOCKET_EVENT_OPEN "open"
#define SYX_SERVER_WEBSOCKET_EVENT_OPEN_CALLBACK "Syx\\Server\\Websocket::onOpen"
#define SYX_SERVER_WEBSOCKET_EVENT_HANDSHAKE "handshake"
#define SYX_SERVER_WEBSOCKET_EVENT_HANDSHAKE_CALLBACK "Syx\\Server\\Websocket::onHandshake"
#define SYX_SERVER_WEBSOCKET_EVENT_MESSAGE "message"
#define SYX_SERVER_WEBSOCKET_EVENT_MESSAGE_CALLBACK "Syx\\Server\\Websocket::onMessage"

extern zend_class_entry *syx_server_websocket_ce;

SYX_STARTUP_FUNCTION(server_websocket);

#endif /* SYX_SERVER_WEBSOCKET_H_ */