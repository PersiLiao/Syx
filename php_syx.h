/*
  +----------------------------------------------------------------------+
  | Yet Another Framework                                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Xinchen Hui  <laruence@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_SYX_H
#define PHP_SYX_H

extern zend_module_entry syx_module_entry;
#define phpext_syx_ptr &syx_module_entry

#ifdef PHP_WIN32
#define PHP_SYX_API __declspec(dllexport)
#ifndef _MSC_VER
#define _MSC_VER 1600
#endif
#else
#define PHP_SYX_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define SYX_G(v) TSRMG(syx_globals_id, zend_syx_globals *, v)
#else
#define SYX_G(v) (syx_globals.v)
#endif

#define PHP_SYX_VERSION 					"1.0.0"

#define SYX_STARTUP_FUNCTION(module)   	ZEND_MINIT_FUNCTION(syx_##module)
#define SYX_RINIT_FUNCTION(module)		ZEND_RINIT_FUNCTION(syx_##module)
#define SYX_STARTUP(module)	 		  	ZEND_MODULE_STARTUP_N(syx_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define SYX_SHUTDOWN_FUNCTION(module)  	ZEND_MSHUTDOWN_FUNCTION(syx_##module)
#define SYX_SHUTDOWN(module)	 	    ZEND_MODULE_SHUTDOWN_N(syx_##module)(INIT_FUNC_ARGS_PASSTHRU)

#define syx_application_t	zval
#define syx_view_t 			zval
#define syx_controller_t	zval
#define syx_request_t		zval
#define syx_router_t		zval
#define syx_route_t			zval
#define syx_dispatcher_t	zval
#define syx_action_t		zval
#define syx_loader_t		zval
#define syx_response_t		zval
#define syx_config_t		zval
#define syx_registry_t		zval
#define syx_plugin_t		zval
#define syx_session_t		zval
#define syx_exception_t		zval

#define SYX_ME(c, m, a, f) {m, PHP_MN(c), a, (uint) (sizeof(a)/sizeof(struct _zend_arg_info)-1), f},

extern PHPAPI void php_var_dump(zval **struc, int level);
extern PHPAPI void php_debug_zval_dump(zval **struc, int level);

ZEND_BEGIN_MODULE_GLOBALS(syx)
    zend_string *namespace;
	zend_string *ext;
	zend_string *base_uri;
	zend_string *directory;
	zend_string *local_library;
	zend_string *local_namespaces;
	zend_string *view_directory;
	zend_string *view_ext;
	zend_string *default_module;
	zend_string *default_controller;
	zend_string *default_action;
	zend_string *bootstrap;
	char         *global_library;
    char         *environ_name;
	zend_bool 	lowcase_path;
	zend_bool 	use_spl_autoload;
	zend_bool 	throw_exception;
	zend_bool   action_prefer;
	zend_bool  	autoload_started;
	zend_bool  	running;
	zend_bool  	in_exception;
	zend_bool  	catch_exception;
	zend_bool   suppressing_warning;
/* {{{ This only effects internally */
	zend_bool  	st_compatible;
/* }}} */
	long		forward_limit;
	HashTable	*configs;
	zval 		 modules;
	zval        *default_route;
	zval        active_ini_file_section;
	zval        *ini_wanted_section;
	uint        parsing_flag;
	zend_bool	use_namespace;
ZEND_END_MODULE_GLOBALS(syx)

PHP_MINIT_FUNCTION(syx);
PHP_MSHUTDOWN_FUNCTION(syx);
PHP_RINIT_FUNCTION(syx);
PHP_RSHUTDOWN_FUNCTION(syx);
PHP_MINFO_FUNCTION(syx);

extern ZEND_DECLARE_MODULE_GLOBALS(syx);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
