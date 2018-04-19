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

#ifndef SYX_SERVER_PLUGIN_H_
#define SYX_SERVER_PLUGIN_H_

#define SYX_SERVER_PLUGIN_HOOK_BEFORE_START "beforestart"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_SHUTDOWN "beforeshutdown"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_MANAGESTART "aftermanagestart"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_MANAGESTOP "beforemanagestop"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_WORKERSTART "afterworkerstart"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_TASKSTART "aftertaskstart"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_WORKERERROR "beforeworkererror"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_WORKEREXIT "beforeworkerexit"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_WORKERSTOP "beforeworkerstop"

#define SYX_SERVER_PLUGIN_HOOK_BEFORE_CONNECT "beforeconnect"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_CONNECT "afterconnect"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_RECEIVE "beforereceive"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_RECEIVE "afterreceive"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_REQUEST "beforerequest"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_REQUEST "afterrequest"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_PACKET "beforepacket"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_PACKET "afterpacket"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_CLOSE "beforeclose"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_TASK "beforetask"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_TASKFINISH "beforetaskfinish"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_TASKFINISH "aftertaskfinish"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_PIPEMESSAGE "beforepipemessage"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_PIPEMESSAGE "afterpipemessage"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_HANDSHAKE "beforehandshake"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_HANDSHAKE "afterhandshake"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_OPEN "beforeopen"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_OPEN "afteropen"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_MESSAGE "beforemessage"
#define SYX_SERVER_PLUGIN_HOOK_AFTER_MESSAGE "aftermessage"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_BUFFERFULL "beforebufferfull"
#define SYX_SERVER_PLUGIN_HOOK_BEFORE_BUFFEREMPTY "beforebufferempty"

extern zend_class_entry *syx_server_plugin_ce;

#define SYX_SERVER_PLUGIN_HANDLE(n) \
    do { \
        zval *p = syx_server_get_plugins(); \
        if (!ZVAL_IS_NULL(p)) { \
            zval *_t_plugin;\
            ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(p), _t_plugin) { \
                if (zend_hash_str_exists(&(Z_OBJCE_P(_t_plugin)->function_table), n, sizeof(n) - 1)) { \
                    zend_call_method_with_2_params(_t_plugin, Z_OBJCE_P(_t_plugin), NULL, n, NULL, zend_read_static_property(syx_dispatcher_ce, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_SERVER), 1), zend_read_static_property(syx_dispatcher_ce, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_SWOOLE_SERVER), 1)); \
                } \
            } ZEND_HASH_FOREACH_END(); \
        } \
    } while(0)

SYX_STARTUP_FUNCTION(server_plugin);

#endif /* SYX_SERVER_PLUGIN_H_ */
