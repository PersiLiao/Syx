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

#ifndef SYX_SERVER_H_
#define SYX_SERVER_H_

#define SYX_SERVER_LOGO  \
"          _____                           _____\n" \
"         / ___/   __  __   _  __         / ___/  ___    _____ _   __  ___    _____\n"   \
"         \\__ \\   / / / /  | |/_/         \\__ \\  / _ \\  / ___/| | / / / _ \\  / ___/ \n"  \
"        ___/ /  / /_/ /  _>  <          ___/ / /  __/ / /    | |/ / /  __/ / /     \n"  \
"       /____/   \\__, /  /_/|_|         /____/  \\___/ /_/     |___/  \\___/ /_/      \n"  \
"               /____/\n\n"


enum SYX_SERVER_MODE
{
    SYX_MODE_BASE          =  1,
    SYX_MODE_THREAD        =  2,
    SYX_MODE_PROCESS       =  3,
    SYX_MODE_SINGLE        =  4,
};

enum SYX_SOCKER_TYPE
{
    SYX_SOCK_TCP          =  1,
    SYX_SOCK_UDP          =  2,
    SYX_SOCK_TCP6         =  3,
    SYX_SOCK_UDP6         =  4,
    SYX_SOCK_UNIX_DGRAM   =  5,  //unix sock dgram
    SYX_SOCK_UNIX_STREAM  =  6,  //unix sock stream
};

#define SYX_SOCK_SSL  (1u << 9)

#define SYX_SERVER_DEFAULT_HOST "127.0.0.1"
#define SYX_SERVER_DEFAULT_PORT "6688"

#define SYX_SERVER_PROPERTY_NAME_SERVER_CLASS "_server_type"
#define SYX_SERVER_PROPERTY_NAME_RUN_SCRIPT_FILE "_run_script_file"
#define SYX_SERVER_PROPERTY_NAME_DAEMONIZE "_daemonize"
#define SYX_SERVER_PROPERTY_NAME_SETTING "_setting"
#define SYX_SERVER_PROPERTY_NAME_PROTOCOL "_protocol"
#define SYX_SERVER_PROPERTY_NAME_PROTOCOL_VERSION "_protocol_version"

#define SYX_SERVER_CLASS_SWOOLE_SERVER "swoole\\server"
#define SYX_SERVER_CLASS_SWOOLE_SERVER_HTTP "swoole\\http\\server"
#define SYX_SERVER_CLASS_SWOOLE_SERVER_WEBSCOKET "swoole\\websocket\\server"
#define SYX_SERVER_CLASS_SWOOLE_HTTP_REQUEST "swoole_http_request"
#define SYX_SERVER_CLASS_SWOOLE_HTTP_RESPONSE "swoole\\http\\response"

#define SYX_SERVER_PROCESS_NAME_MASTER "#%l Master Process"
#define SYX_SERVER_PROCESS_NAME_MANAGER "#%l Manager Process"
#define SYX_SERVER_PROCESS_NAME_WORKER "#%l Worker Process"
#define SYX_SERVER_PROCESS_NAME_TASK "#%l Task Process"

#define SYX_SERVER_EVENT_START "start"
#define SYX_SERVER_EVENT_START_CALLBACK "Syx\\ServerAbstract::onStart"
#define SYX_SERVER_EVENT_MANAGERSTART "managerstart"
#define SYX_SERVER_EVENT_MANAGERSTART_CALLBACK "Syx\\ServerAbstract::onManagerStart"
#define SYX_SERVER_EVENT_MANAGERSTOP "managerstop"
#define SYX_SERVER_EVENT_MANAGERSTOP_CALLBACK "Syx\\ServerAbstract::onManagerStop"
#define SYX_SERVER_EVENT_WORKERSTART "workerstart"
#define SYX_SERVER_EVENT_WORKERSTART_CALLBACK "Syx\\ServerAbstract::onWorkerStart"
#define SYX_SERVER_EVENT_WORKERERROR "workererror"
#define SYX_SERVER_EVENT_WORKERERROR_CALLBACK "Syx\\ServerAbstract::onWorkerError"
#define SYX_SERVER_EVENT_WORKEREXIT "workerexit"
#define SYX_SERVER_EVENT_WORKEREXIT_CALLBACK "Syx\\ServerAbstract::onWorkerExit"
#define SYX_SERVER_EVENT_WORKERSTOP "workerstop"
#define SYX_SERVER_EVENT_WORKERSTOP_CALLBACK "Syx\\ServerAbstract::onWorkerStop"
#define SYX_SERVER_EVENT_CLOSE "close"
#define SYX_SERVER_EVENT_CLOSE_CALLBACK "Syx\\ServerAbstract::onClose"
#define SYX_SERVER_EVENT_TASK "task"
#define SYX_SERVER_EVENT_TASK_CALLBACK "Syx\\ServerAbstract::onTask"
#define SYX_SERVER_EVENT_FINISH "finish"
#define SYX_SERVER_EVENT_FINISH_CALLBACK "Syx\\ServerAbstract::onFinish"
#define SYX_SERVER_EVENT_PIPEMESSAGE "pipemessage"
#define SYX_SERVER_EVENT_PIPEMESSAGE_CALLBACK "Syx\\ServerAbstract::onPipeMessage"
#define SYX_SERVER_EVENT_SHUTDOWN "shutdown"
#define SYX_SERVER_EVENT_SHUTDOWN_CALLBACK "Syx\\ServerAbstract::onShutdown"
#define SYX_SERVER_EVENT_BUFFERFULL "bufferfull"
#define SYX_SERVER_EVENT_BUFFERFULL_CALLBACK "Syx\\ServerAbstract::onBufferFull"
#define SYX_SERVER_EVENT_BUFFEREMPTY "bufferempty"
#define SYX_SERVER_EVENT_BUFFEREMPTY_CALLBACK "Syx\\ServerAbstract::onBufferEmpty"

#define SYX_SERVER_SWOOLE_ON(event_name, name_len, callback, call_len) \
    syx_server_swoole_on(zend_read_static_property(syx_dispatcher_ce, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_SWOOLE_SERVER), 1), event_name, name_len, callback, call_len)

syx_server_t* syx_server_get_server_option_key(const char *key, size_t key_len);
syx_server_t* syx_server_get_server_set_key(const char *key, size_t key_len);
zend_class_entry* syx_server_get_swoole_server_ce(const char *class_name, size_t name_len);

syx_server_t* syx_server_swoole_server_instance(syx_server_t* syx_server_ptr, syx_server_t *syx_swoole_server_ptr, const char *class_name, size_t name_len);
void syx_server_swoole_server_construct(syx_server_t *syx_swoole_server_o, syx_server_t *syx_server_o);
void syx_server_destruct(syx_dispatcher_t *dispatcher, syx_request_t *request, syx_response_t *response);
void syx_server_swoole_on(syx_server_t *syx_swoole_server_o, const char* event_name, size_t name_len, const char* callable_name, size_t callable_name_len);

void syx_server_start(syx_server_t *syx_server_o, syx_server_t *syx_swoole_server_o);
syx_plugin_t* syx_server_get_plugins();

extern zend_class_entry *syx_server_ce;
SYX_STARTUP_FUNCTION(server);

#endif /* SYX_SERVER_H_ */
