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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "main/SAPI.h"
#include "Zend/zend_alloc.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

#include "php_syx.h"
#include "syx_logo.h"
#include "syx_loader.h"
#include "syx_exception.h"
#include "syx_application.h"
#include "syx_dispatcher.h"
#include "syx_config.h"
#include "syx_view.h"
#include "syx_controller.h"
#include "syx_action.h"
#include "syx_request.h"
#include "syx_response.h"
#include "syx_router.h"
#include "syx_bootstrap.h"
#include "syx_plugin.h"
#include "syx_registry.h"
#include "syx_session.h"

ZEND_DECLARE_MODULE_GLOBALS(syx);

/* {{{ syx_functions[]
*/
zend_function_entry syx_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ PHP_INI
 */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("syx.library",         	"",  PHP_INI_ALL, OnUpdateString, global_library, zend_syx_globals, syx_globals)
	STD_PHP_INI_BOOLEAN("syx.action_prefer",   	"0", PHP_INI_ALL, OnUpdateBool, action_prefer, zend_syx_globals, syx_globals)
	STD_PHP_INI_BOOLEAN("syx.lowcase_path",    	"0", PHP_INI_ALL, OnUpdateBool, lowcase_path, zend_syx_globals, syx_globals)
	STD_PHP_INI_BOOLEAN("syx.use_spl_autoload", "0", PHP_INI_ALL, OnUpdateBool, use_spl_autoload, zend_syx_globals, syx_globals)
	STD_PHP_INI_ENTRY("syx.forward_limit", 		"5", PHP_INI_ALL, OnUpdateLongGEZero, forward_limit, zend_syx_globals, syx_globals)
/* {{{ This only effects internally */
	STD_PHP_INI_BOOLEAN("syx.st_compatible",     "0", PHP_INI_ALL, OnUpdateBool, st_compatible, zend_syx_globals, syx_globals)
/* }}} */
	STD_PHP_INI_ENTRY("syx.environ",        	"product", PHP_INI_SYSTEM, OnUpdateString, environ_name, zend_syx_globals, syx_globals)
PHP_INI_END();
/* }}} */

/** {{{ PHP_GINIT_FUNCTION
*/
PHP_GINIT_FUNCTION(syx)
{
	memset(syx_globals, 0, sizeof(*syx_globals));
}
/* }}} */

/** {{{ PHP_MINIT_FUNCTION
*/
PHP_MINIT_FUNCTION(syx)
{
	REGISTER_INI_ENTRIES();

	REGISTER_STRINGL_CONSTANT("SYX\\VERSION", PHP_SYX_VERSION, 	sizeof(PHP_SYX_VERSION) - 1, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRINGL_CONSTANT("SYX\\ENVIRON", SYX_G(environ_name), strlen(SYX_G(environ_name)), CONST_PERSISTENT | CONST_CS);

	REGISTER_LONG_CONSTANT("SYX\\ERR\\STARTUP_FAILED", 		SYX_ERR_STARTUP_FAILED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SYX\\ERR\\ROUTE_FAILED", 		SYX_ERR_ROUTE_FAILED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SYX\\ERR\\DISPATCH_FAILED", 	SYX_ERR_DISPATCH_FAILED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SYX\\ERR\\AUTOLOAD_FAILED", 	SYX_ERR_AUTOLOAD_FAILED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SYX\\ERR\\NOTFOUND\\MODULE", 	SYX_ERR_NOTFOUND_MODULE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SYX\\ERR\\NOTFOUND\\CONTROLLER",SYX_ERR_NOTFOUND_CONTROLLER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SYX\\ERR\\NOTFOUND\\ACTION", 	SYX_ERR_NOTFOUND_ACTION, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SYX\\ERR\\NOTFOUND\\VIEW", 		SYX_ERR_NOTFOUND_VIEW, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SYX\\ERR\\CALL_FAILED",			SYX_ERR_CALL_FAILED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SYX\\ERR\\TYPE_ERROR",			SYX_ERR_TYPE_ERROR, CONST_PERSISTENT | CONST_CS);

	/* startup components */
	SYX_STARTUP(application);
	SYX_STARTUP(bootstrap);
	SYX_STARTUP(dispatcher);
	SYX_STARTUP(loader);
	SYX_STARTUP(request);
	SYX_STARTUP(response);
	SYX_STARTUP(controller);
	SYX_STARTUP(action);
	SYX_STARTUP(config);
	SYX_STARTUP(view);
	SYX_STARTUP(router);
	SYX_STARTUP(plugin);
	SYX_STARTUP(registry);
	SYX_STARTUP(session);
	SYX_STARTUP(exception);

	return SUCCESS;
}
/* }}} */

/** {{{ PHP_MSHUTDOWN_FUNCTION
*/
PHP_MSHUTDOWN_FUNCTION(syx)
{
	UNREGISTER_INI_ENTRIES();

	if (SYX_G(configs)) {
		zend_hash_destroy(SYX_G(configs));
		pefree(SYX_G(configs), 1);
	}

	return SUCCESS;
}
/* }}} */

/** {{{ PHP_RINIT_FUNCTION
*/
PHP_RINIT_FUNCTION(syx)
{
	SYX_G(throw_exception) = 1;
	SYX_G(ext) = zend_string_init(SYX_DEFAULT_EXT, sizeof(SYX_DEFAULT_EXT) - 1, 0);
	SYX_G(view_ext) = zend_string_init(SYX_DEFAULT_VIEW_EXT, sizeof(SYX_DEFAULT_VIEW_EXT) - 1, 0);
	SYX_G(default_module) = zend_string_init(
			SYX_ROUTER_DEFAULT_MODULE, sizeof(SYX_ROUTER_DEFAULT_MODULE) - 1, 0);
	SYX_G(default_controller) = zend_string_init(
			SYX_ROUTER_DEFAULT_CONTROLLER, sizeof(SYX_ROUTER_DEFAULT_CONTROLLER) - 1, 0);
	SYX_G(default_action) = zend_string_init(
			SYX_ROUTER_DEFAULT_ACTION, sizeof(SYX_ROUTER_DEFAULT_ACTION) - 1, 0);
	return SUCCESS;
}
/* }}} */

/** {{{ PHP_RSHUTDOWN_FUNCTION
*/
PHP_RSHUTDOWN_FUNCTION(syx)
{
	SYX_G(running) = 0;
	SYX_G(in_exception)	= 0;
	SYX_G(catch_exception) = 0;

	if (SYX_G(directory)) {
		zend_string_release(SYX_G(directory));
		SYX_G(directory) = NULL;
	}
	if (SYX_G(local_library)) {
		zend_string_release(SYX_G(local_library));
		SYX_G(local_library) = NULL;
	}
	if (SYX_G(local_namespaces)) {
		zend_string_release(SYX_G(local_namespaces));
		SYX_G(local_namespaces) = NULL;
	}
	if (SYX_G(bootstrap)) {
		zend_string_release(SYX_G(bootstrap));
		SYX_G(bootstrap) = NULL;
	}
	if (Z_TYPE(SYX_G(modules)) == IS_ARRAY) {
		zval_ptr_dtor(&SYX_G(modules));
		ZVAL_UNDEF(&SYX_G(modules));
	}
	if (SYX_G(base_uri)) {
		zend_string_release(SYX_G(base_uri));
		SYX_G(base_uri) = NULL;
	}
	if (SYX_G(view_directory)) {
		zend_string_release(SYX_G(view_directory));
		SYX_G(view_directory) = NULL;
	}
	if (SYX_G(view_ext)) {
		zend_string_release(SYX_G(view_ext));
	}
	if (SYX_G(default_module)) {
		zend_string_release(SYX_G(default_module));
	}
	if (SYX_G(default_controller)) {
		zend_string_release(SYX_G(default_controller));
	}
	if (SYX_G(default_action)) {
		zend_string_release(SYX_G(default_action));
	}
	if (SYX_G(ext)) {
		zend_string_release(SYX_G(ext));
	}
	SYX_G(default_route) = NULL;

	return SUCCESS;
}
/* }}} */

/** {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(syx)
{
	php_info_print_table_start();

	php_info_print_table_header(2, "Syx support", "enabled");
	php_info_print_table_row(2, "Version", PHP_SYX_VERSION);
	php_info_print_table_row(2, "Supports", SYX_SUPPORT_URL);
	php_info_print_table_row(2, "Based", "Yaf 3.0.4");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/** {{{ DL support
 */
#ifdef COMPILE_DL_SYX
ZEND_GET_MODULE(syx)
#endif
/* }}} */

/** {{{ module depends
 */
#if ZEND_MODULE_API_NO >= 20050922
zend_module_dep syx_deps[] = {
	ZEND_MOD_REQUIRED("spl")
	ZEND_MOD_REQUIRED("pcre")
	ZEND_MOD_OPTIONAL("session")
	{NULL, NULL, NULL}
};
#endif
/* }}} */

/** {{{ syx_module_entry
*/
zend_module_entry syx_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
	STANDARD_MODULE_HEADER_EX, NULL,
	syx_deps,
#else
	STANDARD_MODULE_HEADER,
#endif
	"syx",
	syx_functions,
	PHP_MINIT(syx),
	PHP_MSHUTDOWN(syx),
	PHP_RINIT(syx),
	PHP_RSHUTDOWN(syx),
	PHP_MINFO(syx),
	PHP_SYX_VERSION,
	PHP_MODULE_GLOBALS(syx),
	PHP_GINIT(syx),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
