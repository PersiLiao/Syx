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

#include "php.h"
#include "Zend/zend_interfaces.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_config.h"
#include "syx_loader.h"
#include "syx_application.h"
#include "syx_dispatcher.h"
#include "syx_router.h"
#include "syx_bootstrap.h"
#include "syx_dispatcher.h"
#include "syx_exception.h"

#include "syx_server.h"
#include "server/syx_server_interface.h"
#include "server/syx_server_plugin.h"
#include "server/syx_server_tcp.h"
#include "server/syx_server_udp.h"
#include "server/syx_server_http.h"
#include "server/syx_server_websocket.h"
#include "server/syx_server_rpc.h"

zend_class_entry *syx_server_ce;

static inline int syx_server_set_proccess_name(syx_zval_t *name){
#ifdef __MACH__
    return SUCCESS;
#endif
    if (Z_STRLEN_P(name) == 0) {
        return SUCCESS;
    }else if (Z_STRLEN_P(name) > 127){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "process name is too long, the max length is 127");
        return FAILURE;
    }else{
        syx_zval_t retval = {{0}}, func_name = {{0}}, params[1];

        ZVAL_COPY(&params[0], name);
        ZVAL_STRING(&func_name, "cli_set_process_title");
        if(call_user_function(EG(function_table), NULL, &func_name, &retval, 1, params) == FAILURE){
            return FAILURE;
        }
        zval_ptr_dtor(params);
        zval_ptr_dtor(&func_name);
        zval_ptr_dtor(&retval);
        return SUCCESS;
    }
}

void syx_server_swoole_on(syx_server_t *syx_swoole_server_o, const char* event_name, size_t event_name_len, const char* callable_name, size_t callable_name_len ){
    syx_server_t func_name ={{0}}, retval = {{0}}, params[2];

    ZVAL_STRING(&func_name, "on");
    ZVAL_STRINGL(&params[0], event_name, event_name_len);
    ZVAL_STRINGL(&params[1], callable_name, callable_name_len);
    zend_call_method_with_2_params(syx_swoole_server_o, Z_OBJCE_P(syx_swoole_server_o), NULL, "on", &retval, &params[0], &params[1]);
    zval_ptr_dtor(params);
    zval_ptr_dtor(&func_name);
    if(&retval){
        zval_ptr_dtor(&retval);
    }
}

void syx_server_swoole_server_construct(syx_server_t *syx_swoole_server_o, syx_server_t *syx_server_o) {
    syx_server_t func_name = {{0}}, retval = {{0}}, retval_x = {{0}};
    syx_server_t *syx_server_set, construct_params[4], set_params[1];

    ZVAL_COPY(&construct_params[0], syx_server_get_server_option_key(ZEND_STRL("host")));
    ZVAL_COPY(&construct_params[1], syx_server_get_server_option_key(ZEND_STRL("port")));
    ZVAL_COPY(&construct_params[2], syx_server_get_server_option_key(ZEND_STRL("mode")));
    ZVAL_COPY(&construct_params[3], syx_server_get_server_option_key(ZEND_STRL("protocol")));

    ZVAL_STRING(&func_name, "__construct");
    call_user_function(NULL, syx_swoole_server_o, &func_name, &retval, 4, construct_params);
    if(&retval){
        zval_ptr_dtor(&retval);
    }
    zval_ptr_dtor(construct_params);
    syx_server_set = syx_server_get_server_option_key(ZEND_STRL("set"));
    if(UNEXPECTED(syx_server_set == NULL)){
        return;
    }
    ZVAL_COPY(&set_params[0], syx_server_set);
    ZVAL_STRING(&func_name, "set");
    zend_call_method_with_1_params(syx_swoole_server_o, Z_OBJCE_P(syx_swoole_server_o), NULL, "set", &retval_x, set_params);
    if(&retval_x){
        zval_ptr_dtor(&retval_x);
    }
    zval_ptr_dtor(set_params);
    zval_ptr_dtor(&func_name);

}

void syx_server_destruct(syx_dispatcher_t *dispatcher, syx_request_t *request, syx_response_t *response){
    syx_router_t *router, rv = {{0}};

    zval_ptr_dtor(request);
    zval_ptr_dtor(response);

    if(Z_TYPE_P(dispatcher) != IS_OBJECT){
        syx_trigger_error(SYX_ERR_TYPE_ERROR, "Must be a %s instance", ZSTR_VAL(syx_dispatcher_ce->name));
    }
    if(Z_TYPE_P(dispatcher) == IS_OBJECT && !instanceof_function(Z_OBJCE_P(dispatcher), syx_dispatcher_ce)){
        syx_trigger_error(SYX_ERR_TYPE_ERROR, "Expect a %s instance, %s give", ZSTR_VAL(syx_dispatcher_ce->name), ZSTR_VAL(Z_OBJCE_P(dispatcher)->name));
    }

    router = syx_router_instance(&rv);
    zend_update_property(syx_dispatcher_ce, dispatcher, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_ROUTER), router);
    zval_ptr_dtor(router);

    zend_update_property_str(syx_dispatcher_ce,
            dispatcher, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_MODULE), SYX_G(default_module));
    zend_update_property_str(syx_dispatcher_ce,
            dispatcher, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_CONTROLLER), SYX_G(default_controller));
    zend_update_property_str(syx_dispatcher_ce,
            dispatcher, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_ACTION), SYX_G(default_action));
}

zend_class_entry* syx_server_get_swoole_server_ce(const char *class_name, size_t name_len){
    zend_string *class_name_lower = zend_string_init(class_name, name_len, 0);
    zend_class_entry *swoole_server_ce = NULL;

    if (UNEXPECTED((swoole_server_ce = zend_hash_find_ptr(EG(class_table), class_name_lower)) == NULL)) {
        syx_trigger_error(SYX_ERR_CALL_FAILED, "%s", "Please install php swoole extension first .");
        zend_string_release(class_name_lower);
        return NULL;
    }
    zend_string_release(class_name_lower);
    return swoole_server_ce;
}

syx_server_t* syx_server_swoole_server_instance(syx_server_t* syx_server_ptr, syx_server_t *syx_swoole_server_ptr, const char *class_name, size_t name_len) {
    syx_server_t* syx_swoole_server_instance;
    zend_class_entry *swoole_server_ce = NULL;

    syx_swoole_server_instance = zend_read_static_property(syx_dispatcher_ce, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_SWOOLE_SERVER), 0);

    if (UNEXPECTED((swoole_server_ce = syx_server_get_swoole_server_ce(class_name, name_len)) == NULL)) {
        return NULL;
    }
    if (IS_OBJECT == Z_TYPE_P(syx_swoole_server_instance) && instanceof_function(Z_OBJCE_P(syx_swoole_server_instance), swoole_server_ce)){
        return syx_swoole_server_instance;
    }
    if (Z_ISUNDEF_P(syx_swoole_server_ptr)) {
        object_init_ex(syx_swoole_server_ptr, swoole_server_ce);
    }
    zend_update_static_property(syx_dispatcher_ce, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_SWOOLE_SERVER), syx_swoole_server_ptr);
    zend_update_static_property(syx_dispatcher_ce, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_SERVER), syx_server_ptr);
    return syx_swoole_server_ptr;
}

syx_plugin_t* syx_server_get_plugins(){
    syx_plugin_t *plugins = NULL;
    syx_dispatcher_t *syx_dispatcher = syx_dispatcher_instance(NULL);
    plugins = zend_read_property(syx_dispatcher_ce, syx_dispatcher, ZEND_STRL(SYX_DISPATCHER_PROPERTY_NAME_PLUGINS), 0, NULL);
    return plugins;
}

void syx_server_start(syx_server_t *syx_server_o, syx_server_t *syx_swoole_server_o) {
    zval func_name={{0}}, retval;

    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_MANAGERSTART), ZEND_STRL(SYX_SERVER_EVENT_MANAGERSTART_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_MANAGERSTOP), ZEND_STRL(SYX_SERVER_EVENT_MANAGERSTOP_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_WORKERSTART), ZEND_STRL(SYX_SERVER_EVENT_WORKERSTART_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_WORKERERROR), ZEND_STRL(SYX_SERVER_EVENT_WORKERERROR_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_WORKEREXIT), ZEND_STRL(SYX_SERVER_EVENT_WORKEREXIT_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_WORKERSTOP), ZEND_STRL(SYX_SERVER_EVENT_WORKERSTOP_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_CLOSE), ZEND_STRL(SYX_SERVER_EVENT_CLOSE_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_TASK), ZEND_STRL(SYX_SERVER_EVENT_TASK_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_FINISH), ZEND_STRL(SYX_SERVER_EVENT_FINISH_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_PIPEMESSAGE), ZEND_STRL(SYX_SERVER_EVENT_PIPEMESSAGE_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_SHUTDOWN), ZEND_STRL(SYX_SERVER_EVENT_SHUTDOWN_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_BUFFERFULL), ZEND_STRL(SYX_SERVER_EVENT_BUFFERFULL_CALLBACK));
    syx_server_swoole_on(syx_swoole_server_o, ZEND_STRL(SYX_SERVER_EVENT_BUFFEREMPTY), ZEND_STRL(SYX_SERVER_EVENT_BUFFEREMPTY_CALLBACK));

    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_START);

    ZVAL_STRING(&func_name, "start");
    call_user_function(NULL, syx_swoole_server_o, &func_name, &retval, 0, NULL);
    zval_ptr_dtor(&func_name);
    if(&retval){
        zval_ptr_dtor(&retval);
    }
}

syx_server_t * syx_server_get_server_set_key(const char *key, size_t key_len){
    syx_server_t *syx_conf, *syx_value;

    syx_conf = syx_server_get_server_option_key(ZEND_STRL("set"));
    if(UNEXPECTED(syx_conf == NULL)){
        return NULL;
    }
    if (UNEXPECTED((syx_value = zend_hash_str_find(Z_ARRVAL_P(syx_conf), key, key_len)) != NULL)) {
        return syx_value;
    }
    return NULL;
}
// TODO 可优化，避免重复读取
syx_server_t * syx_server_get_server_option_key(const char *key, size_t key_len) {
    HashTable *conf, *syx_confz;
    zval *zconf, *syx_conf, *syx_server, *syx_value;

    zconf = zend_read_static_property(syx_server_ce, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_SETTING), 0);
    if (UNEXPECTED(zconf == NULL || Z_TYPE_P(zconf) != IS_OBJECT)) {
        syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s", "get setting error");
        return NULL;
    }
    syx_conf = zend_read_property(syx_config_ce, zconf, ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 0, NULL);
    conf = HASH_OF(syx_conf);
    syx_server = zend_hash_str_find(conf, ZEND_STRL("server"));
    if (UNEXPECTED(syx_server == NULL)) {
        syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s", "get setting->server error");
        return NULL;
    }
    syx_value = zend_hash_str_find(Z_ARRVAL_P(syx_server), key, key_len);
    if (UNEXPECTED((syx_value = zend_hash_str_find(Z_ARRVAL_P(syx_server), key, key_len)) != NULL)) {
        return syx_value;
    }
    return NULL;
}

static int syx_server_parse_option(zval *options) {
    HashTable *conf;
    zval * pzval, *psval, *server;

    conf = HASH_OF(options);
    if (UNEXPECTED((server = zend_hash_str_find(conf, ZEND_STRL("server"))) == NULL)) {
        syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s", "Expected an array of server configure");
        return FAILURE;
    }

    if (UNEXPECTED(Z_TYPE_P(server) != IS_ARRAY)) {
        syx_trigger_error(SYX_ERR_TYPE_ERROR, "%s", "Expected an array of server configure");
        return FAILURE;
    }

    if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(server), ZEND_STRL("host"))) == NULL || Z_TYPE_P(pzval) != IS_STRING || Z_STRLEN_P(pzval) == 0)) {
        syx_trigger_error(SYX_ERR_STARTUP_FAILED, "%s", "Expected a host in server configures");
        return FAILURE;
    }

    if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(server), ZEND_STRL("port"))) == NULL || Z_TYPE_P(pzval) != IS_STRING || Z_STRLEN_P(pzval) == 0)) {
        syx_trigger_error(SYX_ERR_STARTUP_FAILED, "%s", "Expected a port in server configures");
        return FAILURE;
    }

    if (UNEXPECTED((pzval = zend_hash_str_find(Z_ARRVAL_P(server), ZEND_STRL("bootstrap"))) != NULL && Z_TYPE_P(pzval) == IS_STRING)) {
        SYX_G(server_bootstrap) = zend_string_copy(Z_STR_P(pzval));
    }
    if(UNEXPECTED(syx_application_parse_option(options) == FAILURE)){
        return FAILURE;
    }
    return SUCCESS;
}

PHP_METHOD (syx_server, __construct) {
    zval * setting;
    zval * section = NULL, zsection = {{0}}, syx_dispatcher = {{0}};
    syx_config_t zsetting = {{0}};
    syx_server_t syx_swoole_server_o = {{0}}, *server, *self;
    syx_loader_t *loader, zloader = {{0}};

#if PHP_SYX_DEBUG
    php_error_docref(NULL, E_STRICT, "Syx server is running in debug mode");
#endif

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z|z", &setting, &section) == FAILURE) {
        return;
    }

    self = getThis();
    if (!section || Z_TYPE_P(section) != IS_STRING || !Z_STRLEN_P(section)) {
        ZVAL_STRING(&zsection, SYX_G(environ_name));
        (void) syx_config_instance(&zsetting, setting, &zsection);
        zval_ptr_dtor(&zsection);
    } else {
        SYX_G(environ_name) = Z_STRVAL_P(section);
        (void) syx_config_instance(&zsetting, setting, section);
    }

    if (UNEXPECTED(Z_TYPE(zsetting) != IS_OBJECT || syx_server_parse_option(zend_read_property(syx_config_ce, &zsetting, ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 0, NULL)) == FAILURE)) {
        syx_trigger_error(SYX_ERR_STARTUP_FAILED, "Initialization of server config failed");
        zval_ptr_dtor(&zsetting);
        RETURN_FALSE;
    }

    if (SYX_G(local_library)) {
        zend_string *global_library = strlen(SYX_G(global_library))? zend_string_init(SYX_G(global_library), strlen(SYX_G(global_library)), 0) : NULL;
        loader = syx_loader_instance(&zloader, SYX_G(local_library), global_library);
        if (global_library) {
            zend_string_release(global_library);
        }
    } else {
        zend_string *local_library, *global_library;
        local_library = strpprintf(0, "%s%c%s", ZSTR_VAL(SYX_G(directory)), DEFAULT_SLASH, SYX_LIBRARY_DIRECTORY_NAME);
        global_library = strlen(SYX_G(global_library)) ? zend_string_init(SYX_G(global_library), strlen(SYX_G(global_library)), 0) : NULL;
        loader = syx_loader_instance(&zloader, local_library, global_library);
        zend_string_release(local_library);
        if (global_library) {
            zend_string_release(global_library);
        }
    }

    if (UNEXPECTED(Z_TYPE_P(loader) != IS_OBJECT)) {
        syx_trigger_error(SYX_ERR_STARTUP_FAILED, "Initialization of application auto loader failed");
        RETURN_FALSE;
    }
    char *class_name = Z_STRVAL_P(zend_read_property(syx_server_ce, self, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_SERVER_CLASS), 0, NULL));
    syx_server_swoole_server_instance(self, &syx_swoole_server_o, class_name, strlen(class_name));
    (void)syx_dispatcher_instance(&syx_dispatcher);
    zend_update_static_property(syx_server_ce, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_SETTING), &zsetting);
    zval_ptr_dtor(&zsetting);
}

PHP_METHOD (syx_server, __destruct) {
}

PHP_METHOD (syx_server, __sleep) {
}

PHP_METHOD (syx_server, __wakeup) {
}

PHP_METHOD (syx_server, __clone) {
}


PHP_METHOD (syx_server, bootstrap) {
    zend_string *bootstrap_path;
    uint retval = 1;
    zend_class_entry *ce;
    syx_server_t *self = getThis();

    if (!(ce = zend_hash_str_find_ptr(EG(class_table), ZEND_STRL(SYX_DEFAULT_SERVER_BOOTSTRAP_LOWER)))) {
        if (SYX_G(server_bootstrap)) {
            bootstrap_path = zend_string_copy(SYX_G(server_bootstrap));
        } else {
            bootstrap_path = strpprintf(0, "%s%c%s.%s",
                                        ZSTR_VAL(SYX_G(directory)), DEFAULT_SLASH, SYX_DEFAULT_SERVER_BOOTSTRAP,
                                        ZSTR_VAL(SYX_G(ext)));
        }
        if (!syx_loader_import(bootstrap_path, 0)) {
            php_error_docref(NULL, E_WARNING, "Couldn't find bootstrap file %s", ZSTR_VAL(bootstrap_path));
            retval = 0;
        } else if (UNEXPECTED(!instanceof_function(ce, syx_bootstrap_ce))) {
            php_error_docref(NULL, E_WARNING,
                             "Expect a %s instance, %s give", ZSTR_VAL(syx_bootstrap_ce->name), ZSTR_VAL(ce->name));
            retval = 0;
        }
        zend_string_release(bootstrap_path);
    }

    if (UNEXPECTED(!retval)) {
        RETURN_FALSE;
    } else {
        zend_string *func;
        zval bootstrap, *syx_dispatcher;

        object_init_ex(&bootstrap, ce);
        syx_dispatcher = syx_dispatcher_instance(NULL);

        ZEND_HASH_FOREACH_STR_KEY(&(ce->function_table), func)
                {
                    if (strncasecmp(ZSTR_VAL(func), SYX_BOOTSTRAP_INITFUNC_PREFIX,
                                    sizeof(SYX_BOOTSTRAP_INITFUNC_PREFIX) - 1)) {
                        continue;
                    }
                    zend_call_method(&bootstrap, ce, NULL, ZSTR_VAL(func), ZSTR_LEN(func), NULL, 1, syx_dispatcher, NULL);
                    if (UNEXPECTED(EG(exception))) {
                        zval_ptr_dtor(&bootstrap);
                        RETURN_FALSE;
                    }
                }ZEND_HASH_FOREACH_END();
        zval_ptr_dtor(&bootstrap);
    }

    RETURN_ZVAL(self, 1, 0);
}

PHP_METHOD (syx_server, start) {

}

PHP_METHOD (syx_server, stop) {

}

PHP_METHOD (syx_server, reload) {

}

PHP_METHOD (syx_server, restart) {

}

PHP_METHOD(syx_server, getSetting) {
    syx_server_t * setting = zend_read_static_property(syx_server_ce, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_SETTING), 0);
    RETURN_ZVAL(setting, 1, 0);
}

PHP_METHOD(syx_server, setDaemonize) {
    syx_server_t * daemonize = zend_read_property(syx_server_ce, getThis(),
                                                  ZEND_STRL(SYX_SERVER_PROPERTY_NAME_DAEMONIZE), 0, NULL);
    if (UNEXPECTED(IS_TRUE == Z_TYPE_P(daemonize))) {
        RETURN_TRUE;
    }
    ZVAL_BOOL(daemonize, 1);
    RETURN_TRUE;
}

PHP_METHOD(syx_server, onManagerStart){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_AFTER_MANAGESTART);
}

PHP_METHOD(syx_server, onWorkerStart){
    syx_server_t *syx_swoole_server_o = NULL, *syx_server_o, *worker_id, *worker_num, *syx_dispatcher, syx_app_o={{0}};

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zz", &syx_swoole_server_o, &worker_id) == FAILURE) {
        return;
    }

    do{
        object_init_ex(&syx_app_o, syx_application_ce);
        zend_update_property(syx_application_ce, &syx_app_o, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_CONFIG), zend_read_static_property(syx_server_ce, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_SETTING), 0));
        zend_update_property_bool(syx_application_ce, &syx_app_o, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_RUN), 1);
        zend_update_property_string(syx_application_ce, &syx_app_o, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ENV), SYX_G(environ_name));

        if (Z_TYPE(SYX_G(modules)) == IS_ARRAY) {
            zend_update_property(syx_application_ce, &syx_app_o, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_MODULES), &SYX_G(modules));
        } else {
            zend_update_property_null(syx_application_ce, &syx_app_o, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_MODULES));
        }
        zend_update_static_property(syx_application_ce, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_APP), &syx_app_o);
        syx_dispatcher = syx_dispatcher_instance(NULL);
        SYX_BOOTSTRAP_EXEC(syx_dispatcher);
    }while(0);

    worker_num = syx_server_get_server_set_key(ZEND_STRL("worker_num"));
    if(UNEXPECTED(worker_num != NULL)){
         if(Z_LVAL_P(worker_id) >= Z_LVAL_P(worker_num)){
             SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_AFTER_TASKSTART);
         }else{
             SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_AFTER_WORKERSTART);
         }
    }
}

PHP_METHOD(syx_server, onManagerStop){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_MANAGESTOP);
}

PHP_METHOD(syx_server, onWorkerError){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_WORKEREXIT);
}
PHP_METHOD(syx_server, onWorkerExit){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_WORKEREXIT);
}
PHP_METHOD(syx_server, onWorkerStop){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_WORKERSTOP);
}
PHP_METHOD(syx_server, onConnect){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_CONNECT);
}

PHP_METHOD(syx_server, onPacket){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_PACKET);
}
PHP_METHOD(syx_server, onClose){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_CLOSE);
}
PHP_METHOD(syx_server, onTask){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_TASK);
}
PHP_METHOD(syx_server, onFinish){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_TASKFINISH);
}
PHP_METHOD(syx_server, onPipeMessage){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_PIPEMESSAGE);
}
PHP_METHOD(syx_server, onShutdown){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_SHUTDOWN);
}
PHP_METHOD(syx_server, onBufferFull){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_BUFFERFULL);
}
PHP_METHOD(syx_server, onBufferEmpty){
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_BUFFEREMPTY);
}

PHP_METHOD (syx_server, onStop) {
    SYX_SERVER_PLUGIN_HANDLE(SYX_SERVER_PLUGIN_HOOK_BEFORE_SHUTDOWN);
}
/** {{{ syx_server_abstract_methods
 */
zend_function_entry syx_server_abstract_methods[] = {
        PHP_ME(syx_server, __construct, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC)
        PHP_ME(syx_server, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
        PHP_ME(syx_server, __clone, NULL, ZEND_ACC_PRIVATE)
        PHP_ME(syx_server, __sleep, NULL, ZEND_ACC_PRIVATE)
        PHP_ME(syx_server, __wakeup, NULL, ZEND_ACC_PRIVATE)
        PHP_ME(syx_server, bootstrap, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC)
        PHP_ME(syx_server, getSetting, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, setDaemonize, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC)
        PHP_ME(syx_server, start, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT)
        PHP_ME(syx_server, reload, syx_server_interface_reload_arginfo, ZEND_ACC_PUBLIC)
        PHP_ME(syx_server, restart, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC)
        PHP_ME(syx_server, stop, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC)
        PHP_ME(syx_server, onManagerStart, syx_server_interface_managerstart_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onManagerStop, syx_server_interface_managerstop_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onWorkerStart, syx_server_interface_workerstart_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onWorkerError, syx_server_interface_workererror_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onWorkerExit, syx_server_interface_workerexit_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onWorkerStop, syx_server_interface_workerstop_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onConnect, syx_server_interface_connect_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onPacket, syx_server_interface_packet_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onClose, syx_server_interface_close_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onTask, syx_server_interface_task_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onFinish, syx_server_interface_finish_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onPipeMessage, syx_server_interface_pipemessage_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onShutdown, syx_server_interface_shutdown_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onBufferFull, syx_server_interface_bufferfull_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onBufferEmpty, syx_server_interface_bufferempty_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(syx_server, onStop, syx_server_interface_void_arginfo, ZEND_ACC_PUBLIC)
        {NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
 */
SYX_STARTUP_FUNCTION(server) {
    zend_class_entry ce;

    SYX_STARTUP(server_interface);
    SYX_STARTUP(server_plugin);

    SYX_INIT_CLASS_ENTRY(ce, "Syx\\ServerAbstract", syx_server_abstract_methods);
    syx_server_ce = zend_register_internal_class(&ce);
    zend_class_implements(syx_server_ce, 1, syx_server_interface_ce);
    syx_server_ce->ce_flags = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

    zend_declare_property_bool(syx_server_ce, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_DAEMONIZE), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_null(syx_server_ce, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_APP), ZEND_ACC_STATIC | ZEND_ACC_PROTECTED);
    zend_declare_property_null(syx_server_ce, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_SETTING), ZEND_ACC_STATIC | ZEND_ACC_PROTECTED);
    zend_declare_property_string(syx_server_ce, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_RUN_SCRIPT_FILE), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(syx_server_ce, ZEND_STRL(SYX_SERVER_PROPERTY_NAME_SERVER_CLASS), SYX_SERVER_CLASS_SWOOLE_SERVER, ZEND_ACC_PROTECTED);

    SYX_STARTUP(server_tcp);
//    SYX_STARTUP(server_udp);
    SYX_STARTUP(server_http);
    SYX_STARTUP(server_websocket);
    return SUCCESS;
}
