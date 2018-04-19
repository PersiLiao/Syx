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

#ifndef SYX_REQUEST_SERVER_WEBSOCKET_H_
#define SYX_REQUEST_SERVER_WEBSOCKET_H_

#define SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_FRAME  "_frame"
#define SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_FINISH "_finish"
#define SYX_REQUEST_PROPERTY_NAME_SWOOL_WEBSOCKET_OPCODE "_opcode"

enum syx_websocket_opcode
{
    SYX_WEBSOCKET_OPCODE_CONTINUATION_FRAME = 0x0,
    SYX_WEBSOCKET_OPCODE_TEXT_FRAME = 0x1,
    SYX_WEBSOCKET_OPCODE_BINARY_FRAME = 0x2,
    SYX_WEBSOCKET_OPCODE_CONNECTION_CLOSE = 0x8,
    SYX_WEBSOCKET_OPCODE_PING = 0x9,
    SYX_WEBSOCKET_OPCODE_PONG = 0xa,

    SYX_WEBSOCKET_CLOSE_NORMAL = 1000,
    SYX_WEBSOCKET_CLOSE_GOING_AWAY = 1001,
    SYX_WEBSOCKET_CLOSE_PROTOCOL_ERROR = 1002,
    SYX_WEBSOCKET_CLOSE_DATA_ERROR = 1003,
    SYX_WEBSOCKET_CLOSE_STATUS_ERROR = 1005,
    SYX_WEBSOCKET_CLOSE_ABNORMAL = 1006,
    SYX_WEBSOCKET_CLOSE_MESSAGE_ERROR = 1007,
    SYX_WEBSOCKET_CLOSE_POLICY_ERROR = 1008,
    SYX_WEBSOCKET_CLOSE_MESSAGE_TOO_BIG = 1009,
    SYX_WEBSOCKET_CLOSE_EXTENSION_MISSING = 1010,
    SYX_WEBSOCKET_CLOSE_SERVER_ERROR = 1011,
    SYX_WEBSOCKET_CLOSE_TLS = 1015,
    SYX_WEBSOCKET_VERSION = 13,
};

extern zend_class_entry* syx_request_swoole_websocket_ce;
syx_request_t *syx_request_swoole_websocket_instance(syx_request_t *this_ptr, syx_request_t *swoole_websocket_frame);

SYX_STARTUP_FUNCTION(request_swoole_websocket);

#endif /* SYX_REQUEST_SERVER_WEBSOCKET_H_ */
