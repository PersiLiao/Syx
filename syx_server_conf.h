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

#ifndef SYX_SERVER_CONF_H_
#define SYX_SERVER_CONF_H_

#define SYX_SERVER_CONF_PROPERTY_NAME_DAEMONIZE "daemonize"
#define SYX_SERVER_CONF_PROPERTY_NAME_ENABLE_SSL "enable_ssl"
#define SYX_SERVER_CONF_PROPERTY_NAME_WORKER_NUM "worker_num"
#define SYX_SERVER_CONF_PROPERTY_NAME_TASK_NUM "task_num"
#define SYX_SERVER_CONF_PROPERTY_NAME_MAX_REQUEST "max_request"
#define SYX_SERVER_CONF_PROPERTY_NAME_ACTION "action"
#define SYX_SERVER_CONF_PROPERTY_NAME_TYPE "type"
#define SYX_SERVER_CONF_PROPERTY_NAME_HOST "host"
#define SYX_SERVER_CONF_PROPERTY_NAME_PORT "port"
#define SYX_SERVER_CONF_PROPERTY_NAME_PID "pid"
#define SYX_SERVER_CONF_PROPERTY_NAME_LOG "log"

typedef struct _syx_server_conf_struct{
    zend_bool daemonize;
    zend_bool enable_ssl;
    zend_long worker_num;
    zend_long task_num;
    zend_long max_request;
    zend_long action;
    zend_string *type;
    zend_string *host;
    zend_string *port;
    zend_string *pid;
    zend_string *log;
}syx_server_conf;

#endif /* SYX_SERVER_CONF_H_ */
