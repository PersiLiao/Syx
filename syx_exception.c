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
#include "Zend/zend_exceptions.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_application.h"
#include "syx_exception.h"

zend_class_entry *syx_ce_RuntimeException;
zend_class_entry *syx_exception_ce;

zend_class_entry *syx_buildin_exceptions[SYX_MAX_BUILDIN_EXCEPTION];

/** {{{void syx_trigger_error(int type, char *format, ...)
 */
void syx_trigger_error(int type, char *format, ...) {
	va_list args;
	char *message;
	uint msg_len;

	va_start(args, format);
	msg_len = vspprintf(&message, 0, format, args);
	va_end(args);

	if (SYX_G(throw_exception)) {
		syx_throw_exception(type, message);
	} else {
		syx_application_t *app = zend_read_static_property(syx_application_ce, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_APP), 1);
		zend_update_property_long(syx_application_ce, app, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ERRNO), type);
		zend_update_property_stringl(syx_application_ce, app, ZEND_STRL(SYX_APPLICATION_PROPERTY_NAME_ERRMSG), message, msg_len);
		php_error_docref(NULL, E_RECOVERABLE_ERROR, "%s", message);
	}
	efree(message);
}
/* }}} */

/** {{{ zend_class_entry * syx_get_exception_base(int root)
*/
zend_class_entry * syx_get_exception_base(int root) {
#if can_handle_soft_dependency_on_SPL && defined(HAVE_SPL) && ((PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 1))
	if (!root) {
		if (!spl_ce_RuntimeException) {
			zend_class_entry **pce;

			if (zend_hash_find(CG(class_table), "runtimeexception", sizeof("RuntimeException"), (void **) &pce) == SUCCESS) {
				spl_ce_RuntimeException = *pce;
				return *pce;
			}
		} else {
			return spl_ce_RuntimeException;
		}
	}
#endif

	return zend_exception_get_default();
}
/* }}} */

/** {{{ void syx_throw_exception(long code, char *message)
*/
void syx_throw_exception(long code, char *message) {
	zend_class_entry *base_exception = syx_exception_ce;

	if ((code & SYX_ERR_BASE) == SYX_ERR_BASE
			&& syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(code)]) {
		base_exception = syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(code)];
	}

	zend_throw_exception(base_exception, message, code);
}
/* }}} */

/** {{{ syx_exception_methods
*/
zend_function_entry syx_exception_methods[] = {
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 3)) || (PHP_MAJOR_VERSION < 5)
	PHP_ME(syx_exception, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(syx_exception, getPrevious, NULL, ZEND_ACC_PUBLIC)
#endif
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(exception) {
	zend_class_entry ce;
	zend_class_entry startup_ce;
	zend_class_entry route_ce;
	zend_class_entry dispatch_ce;
	zend_class_entry loader_ce;
	zend_class_entry module_notfound_ce;
	zend_class_entry controller_notfound_ce;
	zend_class_entry action_notfound_ce;
	zend_class_entry view_notfound_ce;
	zend_class_entry type_ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Exception", syx_exception_methods);
	syx_exception_ce = zend_register_internal_class_ex(&ce, syx_get_exception_base(0));
	zend_declare_property_null(syx_exception_ce, ZEND_STRL("message"), 	ZEND_ACC_PROTECTED);
	zend_declare_property_long(syx_exception_ce, ZEND_STRL("code"), 0,	ZEND_ACC_PROTECTED);
	zend_declare_property_null(syx_exception_ce, ZEND_STRL("previous"),  ZEND_ACC_PROTECTED);

	SYX_INIT_CLASS_ENTRY(startup_ce, "Syx\\Exception\\StartupError", NULL);
	syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_STARTUP_FAILED)] = zend_register_internal_class_ex(&startup_ce, syx_exception_ce);

	SYX_INIT_CLASS_ENTRY(route_ce, "Syx\\Exception\\RouterFailed", NULL);
	syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_ROUTE_FAILED)] = zend_register_internal_class_ex(&route_ce, syx_exception_ce);

	SYX_INIT_CLASS_ENTRY(dispatch_ce, "Syx\\Exception\\DispatchFailed", NULL);
	syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_DISPATCH_FAILED)] = zend_register_internal_class_ex(&dispatch_ce, syx_exception_ce);

	SYX_INIT_CLASS_ENTRY(loader_ce, "Syx\\Exception\\LoadFailed", NULL);
	syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_AUTOLOAD_FAILED)] = zend_register_internal_class_ex(&loader_ce, syx_exception_ce);

	SYX_INIT_CLASS_ENTRY(module_notfound_ce, "Syx\\Exception\\LoadFailed\\Module", NULL);
	syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_NOTFOUND_MODULE)] = zend_register_internal_class_ex(&module_notfound_ce, syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_AUTOLOAD_FAILED)]);

	SYX_INIT_CLASS_ENTRY(controller_notfound_ce, "Syx\\Exception\\LoadFailed\\Controller", NULL);
	syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_NOTFOUND_CONTROLLER)] = zend_register_internal_class_ex(&controller_notfound_ce, syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_AUTOLOAD_FAILED)]);

	SYX_INIT_CLASS_ENTRY(action_notfound_ce,  "Syx\\Exception\\LoadFailed\\Action", NULL);
	syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_NOTFOUND_ACTION)] = zend_register_internal_class_ex(&action_notfound_ce, syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_AUTOLOAD_FAILED)]);

	SYX_INIT_CLASS_ENTRY(view_notfound_ce, "Syx\\Exception\\LoadFailed\\View", NULL);
	syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_NOTFOUND_VIEW)] = zend_register_internal_class_ex(&view_notfound_ce, syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_AUTOLOAD_FAILED)]);

	SYX_INIT_CLASS_ENTRY(type_ce, "Syx\\Exception\\TypeError", NULL);
	syx_buildin_exceptions[SYX_EXCEPTION_OFFSET(SYX_ERR_TYPE_ERROR)] = zend_register_internal_class_ex(&type_ce, syx_exception_ce);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
