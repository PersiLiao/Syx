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

#ifndef SYX_SERVER_INTERFACE_H_
#define SYX_SERVER_INTERFACE_H_

#define SYX_SERVER_TYPE_SWOOLE "swoole"

#define SYX_SERVER_TYPE_SWOOLE_TCP "swoole_tcp"
#define SYX_SERVER_TYPE_SWOOLE_UDP "swoole_udp"
#define SYX_SERVER_TYPE_SWOOLE_HTTP "swoole_http"
#define SYX_SERVER_TYPE_SWOOLE_WEBSOCKET "swoole_websocket"
#define SYX_SERVER_TYPE_SWOOLE_RPC "swoole_rpc"

extern zend_class_entry *syx_server_interface_ce;

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_void_arginfo, 0, 0, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_reload_arginfo, 1, 0, 0)
    SYX_ARG_INFO(0, only_task)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_managerstart_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_managerstop_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_workerstart_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, swoole_server, Swoole\\Server, 0)
    SYX_ARG_INFO(0, worker_id)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_workererror_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_workerexit_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_workerstop_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_connect_arginfo, 0, 0, 3)
    SYX_ARG_OBJ_INFO(0, server, Swoole\\Server, 0)
    SYX_ARG_INFO(0, fd)
    SYX_ARG_INFO(0, reactorId)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_receive_arginfo, 0, 0, 4)
    SYX_ARG_OBJ_INFO(0, swoole_server, Swoole\\Server, 0)
    SYX_ARG_INFO(0, fd)
    SYX_ARG_INFO(0, reactor_id)
    SYX_ARG_INFO(0, data)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_request_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, request, Swoole\\Http\\Request, 0)
    SYX_ARG_OBJ_INFO(0, response, Swoole\\Http\\Response, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_packet_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_close_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_task_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_finish_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_pipemessage_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_shutdown_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_bufferfull_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_bufferempty_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_open_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_handshake_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, server, Syx\\ServerAbstract, 0)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_server_interface_message_arginfo, 0, 0, 2)
    SYX_ARG_OBJ_INFO(0, sw_server, Swoole\\Server, 0)
    SYX_ARG_OBJ_INFO(0, message, Swoole\\Websocket\\Frame, 0)
SYX_END_ARG_INFO()

SYX_STARTUP_FUNCTION(server);

#endif /* SYX_SERVER_INTERFACE_H_ */
