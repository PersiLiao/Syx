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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "string.h"
#include "php.h"
#include "php_getopt.h"
#include "Zend/zend_interfaces.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_application.h"
#include "syx_dispatcher.h"
#include "syx_exception.h"

#include "syx_request.h"
#include "requests/syx_request_http.h"

#include "syx_server.h"
#include "library/base.h"
#include "library/console.h"
#include "syx_command.h"

static inline int syx_command_handler_init(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    return SUCCESS;
}

static inline int syx_command_handler_daemonize(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    serv_conf->daemonize = IS_TRUE;
    return SUCCESS;
}

static inline int syx_command_handler_host(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    serv_conf->host = zend_string_init(Z_STRVAL_P(val), Z_STRLEN_P(val), 0);
    return SUCCESS;
}

static inline int syx_command_handler_port(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    serv_conf->port = zend_string_init(Z_STRVAL_P(val), Z_STRLEN_P(val), 0);
    return SUCCESS;
}

static inline int syx_command_handler_log(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    serv_conf->log = zend_string_init(Z_STRVAL_P(val), Z_STRLEN_P(val), 0);
    return SUCCESS;
}

static inline int syx_command_handler_ini(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    return SUCCESS;
}

static inline int syx_command_handler_pid(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    return SUCCESS;
}

static inline int syx_command_handler_app_dir(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    return SUCCESS;
}

static inline int syx_command_handler_conf(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    zend_string *conf;

    if(Z_STRVAL_P(val)[0] != '/' && Z_STRVAL_P(val)[0] != '\\'){
        conf = strpprintf(0, "%s%c%s", syx_app_cwd, DEFAULT_SLASH, Z_STRVAL_P(val));
    }else{
        conf = zend_string_init(Z_STRVAL_P(val), Z_STRLEN_P(val), 0);
    }
    app_conf->conf = conf;
    zend_string_release(conf);
    return SUCCESS;
}

static inline int syx_command_handler_server(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    if(strcasecmp("tcp", Z_STRVAL_P(val)) != 0 && strcasecmp("udp", Z_STRVAL_P(val)) != 0 && strcasecmp("http", Z_STRVAL_P(val)) != 0 && strcasecmp("websocket", Z_STRVAL_P(val)) != 0 && strcasecmp("rpc", Z_STRVAL_P(val)) != 0 ){
        syx_console_output("invalid server type, please try option --h \n", SYX_CONSOLE_TYPE_ERROR);
        exit(0);
    }
    serv_conf->type = zend_string_init(Z_STRVAL_P(val), Z_STRLEN_P(val), 0);
    return SUCCESS;
}

static inline int syx_command_handler_enable_ssl(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    serv_conf->enable_ssl = IS_TRUE;
    return SUCCESS;
}

static inline int syx_command_handler_worker_num(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    if(Z_TYPE_P(val) == IS_LONG){
        serv_conf->worker_num = Z_LVAL_P(val);
    }else if(Z_TYPE_P(val) == IS_STRING){
        convert_to_long(val);
        serv_conf->worker_num = Z_LVAL_P(val);
    }else{
        fprintf(stdout, "worker num type is %d", Z_TYPE_P(val));
        return FAILURE;
    }
    return SUCCESS;
}

static inline int syx_command_handler_max_request(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    convert_to_long(val);
    serv_conf->max_request = Z_LVAL_P(val);
    return SUCCESS;
}

static inline int syx_command_handler_task_num(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    if(Z_TYPE_P(val) == IS_LONG){
        serv_conf->task_num = Z_LVAL_P(val);
    }else if(Z_TYPE_P(val) == IS_STRING){
        convert_to_long(val);
        serv_conf->task_num = Z_LVAL_P(val);
    }else{
        fprintf(stdout, "worker num type is %d", Z_TYPE_P(val));
        return FAILURE;
    }
    return SUCCESS;
}

static inline int syx_command_handler_start(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    if(serv_conf->action){
        syx_console_output("Can only execute one command at a time. \n", SYX_CONSOLE_TYPE_ERROR );
        exit(0);
    }
    serv_conf->action = SYX_COMMAND_ACTION_START;
    return SUCCESS;
}

static inline int syx_command_handler_reload(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    if(serv_conf->action){
        syx_console_output("Can only execute one command at a time. \n", SYX_CONSOLE_TYPE_ERROR );
        exit(0);
    }
    serv_conf->action = SYX_COMMAND_ACTION_RELOAD;
    return SUCCESS;
}

static inline int syx_command_handler_reload_all(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    if(serv_conf->action){
        syx_console_output("Can only execute one command at a time ", SYX_CONSOLE_TYPE_ERROR );
        exit(0);
    }
    serv_conf->action = SYX_COMMAND_ACTION_RELOAD_ALL;
    return SUCCESS;
}

static inline int syx_command_handler_restart(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    if(serv_conf->action){
        syx_console_output("Can only execute one command at a time. \n", SYX_CONSOLE_TYPE_ERROR );
        exit(0);
    }
    serv_conf->action = SYX_COMMAND_ACTION_RESTART;
    return SUCCESS;
}

static inline int syx_command_handler_stop(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    if(serv_conf->action){
        syx_console_output("Can only execute one command at a time. \n", SYX_CONSOLE_TYPE_ERROR );
        exit(0);
    }
    serv_conf->action = SYX_COMMAND_ACTION_STOP;
    return SUCCESS;
}

static inline int syx_command_handler_usage(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    fprintf(stdout, "%s\nsyx version: syx/%s\n  \
    Usage:  syx [option] [-?vdhp] [--enable-ssl] [--type=server type] [--worker-num=number] [--max-request=number] [--task-num=number] ... \n \
    \n \
    Options:    \n\
      -?,--h                  : show this help \n\
      -v                      : show version and exit \n\
      -d                      : set server run backgroud (default: no) \n\
      -h=host                 : set server bind address (default: 127.0.0.1) \n\
      -p=port                 : set server listen port (default: 9501) \n\
      --enable-ssl            : set server enable ssl, with openssl (default: disabled) \n\
      --type=server type      : set server type (default: http), example: tcp, udp, http, websocket, rpc \n\
      --worker-num=number     : set server worker process num (default: cpu num) \n\
      --max-request=number    : set server worker process max request num (default: 0) \n\
      --task-num=number       : set server task process num (default: 0) \n\
      --pid=filename          : set server master process pid filename (default: no) \n\
      start                   : start server \n\
      reload                  : reload worker process \n\
      reload-all              : reload worker and task process \n\
      restart                 : restart server \n\
      stop                    : stop server \n\n\n", SYX_SERVER_LOGO, PHP_SYX_VERSION);
    exit(0);

//    --prefix=directory      : set application root directory (default: __DIR__) \n\
//    -c,--conf=filename      : set application config filename (default: __DIR__ . /conf/application.ini) \n\
//    --env=environ           : set application config environ name (default: product) \n\

}

static inline int syx_command_handler_version(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    fprintf(stdout, "syx version %s\n", PHP_SYX_VERSION);
    exit(0);
    return SUCCESS;
}

static inline int syx_command_handler_error(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd){
    return SUCCESS;
}

const syx_opt_struct SYX_COMMAND_OPTIONS[] = {
    {"syx", 0, 0, "syx", syx_command_handler_init},
    {"v", 1, 0, "version", syx_command_handler_version},
    {"d", 1, 0, "daemonize", syx_command_handler_daemonize}, // 默认为0
    {"h", 1, 1, "host", syx_command_handler_host}, // 默认127.0.0.1
    {"p", 1, 1, "port", syx_command_handler_port}, // 默认为6688
    {"log", 1, 2, "log", syx_command_handler_log},
    {"i", 0, 0, "ini", syx_command_handler_ini},
    {"pid", 1, 2, "pid", syx_command_handler_pid},
    {"app-dir", 1, 1, "app_dir", syx_command_handler_app_dir},
    {"c", 1, 1, "conf", syx_command_handler_conf},
    {"conf", 1, 1, "conf", syx_command_handler_conf},
    {"s", 1, 1, "server", syx_command_handler_server},
    {"type", 2, 1, "server_type", syx_command_handler_server},
    {"enable-ssl", 2, 0, "enable_ssl", syx_command_handler_enable_ssl},
    {"worker-num", 2, 1, "worker_num", syx_command_handler_worker_num},
    {"task-num", 2, 1, "task_num", syx_command_handler_task_num},
    {"max-request", 2, 1, "max_request", syx_command_handler_max_request},
    {"start", 0, 0, "start", syx_command_handler_start},
    {"reload", 0, 0, "reload", syx_command_handler_reload},
    {"reload-all", 0, 0, "reload_all", syx_command_handler_reload_all},
    {"restart", 0, 0, "restart", syx_command_handler_restart},
    {"stop", 0, 0, "stop", syx_command_handler_stop},
    {"?", 1, 0, "usage", syx_command_handler_usage},/* help alias (both '?' and 'usage') */
    {"h", 2, 0, "help", syx_command_handler_usage}
};

static inline syx_zval_t* syx_command_get_val(syx_opt_struct opt, char *str, size_t len){

    int opt_len = strlen(opt.opt_char) + opt.opt_len + strlen(SYX_COMMAND_ASSIGN_OPERATORS), val_len = len - opt_len;
    if(val_len < 0){
        syx_console_output("syx: invalid option, please try option --h \n", SYX_CONSOLE_TYPE_ERROR);
        exit(0);
    }
    char *val = NULL, *val_str = str + opt_len;
    val = strtok(str, SYX_COMMAND_ASSIGN_OPERATORS);
    val = strtok(NULL, SYX_COMMAND_ASSIGN_OPERATORS);
    syx_zval_t syx_value = {{0}};
    ZVAL_STRINGL(&syx_value, val, strlen(val));
    return &syx_value;
}

void syx_command_parse(){
    char *syx_app_cwd = getcwd(NULL, 0);
    int syx_opts_len = SYX_C_GET_ARRAY_LEN(SYX_COMMAND_OPTIONS);
    syx_zval_t *syx_argv, *syx_azval, *syx_opt_val;
    zend_long syx_index, check_flag = 0;

    syx_argv = syx_request_query_str(SYX_GLOBAL_VARS_SERVER, "argv", sizeof("argv") -1);
    if (syx_argv && IS_ARRAY == Z_TYPE_P(syx_argv)) {
        ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(syx_argv), syx_index, syx_azval) {
            if(syx_index == 0){
                if(!SYX_COMMAND_OPT_CHECK(syx_azval, 0, "syx")){
                    break;
                }else{
                    continue;
                }
            }
            for(int i = 0; i < syx_opts_len; i++){
                if(SYX_COMMAND_OPT_CHECK(syx_azval, SYX_COMMAND_OPTIONS[i].opt_len, SYX_COMMAND_OPTIONS[i].opt_char)){
                    if(SYX_COMMAND_OPTIONS[i].need_param == 1){
                        if((syx_opt_val = syx_command_get_val(SYX_COMMAND_OPTIONS[i], Z_STRVAL_P(syx_azval), Z_STRLEN_P(syx_azval))) != FAILURE){
                            SYX_COMMAND_OPTIONS[i].handler(&SYX_G(serv_conf), &SYX_G(app_conf), syx_opt_val, syx_app_cwd);
                        }else{
                            break;
                        }
                    }else{
                        SYX_COMMAND_OPTIONS[i].handler(&SYX_G(serv_conf), &SYX_G(app_conf), NULL, syx_app_cwd);
                    }
                    check_flag = 1;
                    break;
                }
            }
            if(!check_flag){
                fprintf(stdout, "syx: invalid option: \"%s\", please try option --h \n", Z_STRVAL_P(syx_azval));
                exit(0);
            }else{
                check_flag = 0;
            }
        } ZEND_HASH_FOREACH_END();
    }
}