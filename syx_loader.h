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

#ifndef SYX_LOADER_H
#define SYX_LOADER_H

#define SYX_DEFAULT_VIEW_EXT     	  		"html"
#define SYX_DEFAULT_LIBRARY_EXT		   		SYX_DEFAULT_CONTROLLER_EXT

#define SYX_LIBRARY_DIRECTORY_NAME    		"library"
#define SYX_CONTROLLER_DIRECTORY_NAME 		"controller"
#define SYX_PLUGIN_DIRECTORY_NAME 	  		"plugin"
#define SYX_MODULE_DIRECTORY_NAME     		"module"
#define SYX_VIEW_DIRECTORY_NAME       		"view"
#define SYX_MODEL_DIRECTORY_NAME      		"model"

#define SYX_SPL_AUTOLOAD_REGISTER_NAME 		"spl_autoload_register"
#define SYX_AUTOLOAD_FUNC_NAME 				"autoload"
#define SYX_LOADER_PROPERTY_NAME_INSTANCE	"_instance"
#define SYX_LOADER_PROPERTY_NAME_NAMESPACE	"_local_ns"

#define SYX_LOADER_CONTROLLER				"Controller"
#define SYX_LOADER_LEN_CONTROLLER			10
#define SYX_LOADER_MODEL					"Model"
#define SYX_LOADER_LEN_MODEL				5
#define SYX_LOADER_PLUGIN					"Plugin"
#define SYX_LOADER_LEN_PLUGIN				6
#define SYX_LOADER_RESERVERD				"Syx_"
#define SYX_LOADER_LEN_RESERVERD			3

/* {{{ This only effects internally */
#define SYX_LOADER_DAO						"Dao_"
#define SYX_LOADER_LEN_DAO					4
#define SYX_LOADER_SERVICE					"Service_"
#define SYX_LOADER_LEN_SERVICE				8
/* }}} */

#define	SYX_LOADER_PROPERTY_NAME_LIBRARY	"_library"
#define SYX_LOADER_PROPERTY_NAME_GLOBAL_LIB "_global_library"

#define SYX_STORE_EG_ENVIRON() \
	{ \
		zval ** __old_return_value_pp   = EG(return_value_ptr_ptr); \
		zend_op ** __old_opline_ptr  	= EG(opline_ptr); \
		zend_op_array * __old_op_array  = EG(active_op_array);

#define SYX_RESTORE_EG_ENVIRON() \
		EG(return_value_ptr_ptr) = __old_return_value_pp;\
		EG(opline_ptr)			 = __old_opline_ptr; \
		EG(active_op_array)		 = __old_op_array; \
	}

extern zend_class_entry *syx_loader_ce;

int syx_internal_autoload(char *file_name, size_t name_len, char **directory);
int syx_loader_import(zend_string *path, int use_path);
int syx_register_autoloader(syx_loader_t *loader);
int syx_loader_register_namespace_single(char *prefix, size_t len);
syx_loader_t *syx_loader_instance(syx_loader_t *this_ptr, zend_string *library_path, zend_string *global_path);

extern PHPAPI int php_stream_open_for_zend_ex(const char *filename, zend_file_handle *handle, int mode);

SYX_STARTUP_FUNCTION(loader);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
