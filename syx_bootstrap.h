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

#ifndef SYX_BOOTSTRAP_H
#define SYX_BOOTSTRAP_H

#define SYX_DEFAULT_BOOTSTRAP		  	"Bootstrap"
#define SYX_DEFAULT_SERVER_BOOTSTRAP           "ServerBootstrap"
#define SYX_DEFAULT_BOOTSTRAP_LOWER	  	"bootstrap"
#define SYX_DEFAULT_SERVER_BOOTSTRAP_LOWER     "serverbootstrap"
#define SYX_BOOTSTRAP_INITFUNC_PREFIX  	"_init"

#define SYX_BOOTSTRAP_EXEC(syx_dipatcher) do{ \
    zend_string *bootstrap_path; \
    uint  retval = 1; \
    zend_class_entry  *ce; \
    if (!(ce = zend_hash_str_find_ptr(EG(class_table), ZEND_STRL(SYX_DEFAULT_BOOTSTRAP_LOWER)))) { \
        if (SYX_G(bootstrap)) { \
            bootstrap_path = zend_string_copy(SYX_G(bootstrap)); \
        } else { \
            bootstrap_path = strpprintf(0, "%s%c%s.%s", \
                    ZSTR_VAL(SYX_G(directory)), DEFAULT_SLASH, SYX_DEFAULT_BOOTSTRAP, ZSTR_VAL(SYX_G(ext))); \
        } \
        if (!syx_loader_import(bootstrap_path, 0)) { \
            php_error_docref(NULL, E_WARNING, "Couldn't find bootstrap file %s", ZSTR_VAL(bootstrap_path)); \
            retval = 0; \
        } else if (UNEXPECTED((ce = zend_hash_str_find_ptr(EG(class_table), ZEND_STRL(SYX_DEFAULT_BOOTSTRAP_LOWER))) == NULL)) { \
            php_error_docref(NULL, E_WARNING, "Couldn't find class %s in %s", SYX_DEFAULT_BOOTSTRAP, ZSTR_VAL(bootstrap_path)); \
            retval = 0; \
        } else if (UNEXPECTED(!instanceof_function(ce, syx_bootstrap_ce))) { \
            php_error_docref(NULL, E_WARNING, "Expect a %s instance, %s give", ZSTR_VAL(syx_bootstrap_ce->name), ZSTR_VAL(ce->name)); \
            retval = 0; \
        } \
        zend_string_release(bootstrap_path); \
    } \
    if (retval) { \
        zend_string *func; \
        zval bootstrap; \
        object_init_ex(&bootstrap, ce); \
        ZEND_HASH_FOREACH_STR_KEY(&(ce->function_table), func) { \
            if (strncasecmp(ZSTR_VAL(func), SYX_BOOTSTRAP_INITFUNC_PREFIX, sizeof(SYX_BOOTSTRAP_INITFUNC_PREFIX)-1)) { \
                continue; \
            } \
            zend_call_method(&bootstrap, ce, NULL, ZSTR_VAL(func), ZSTR_LEN(func), NULL, 1, syx_dipatcher, NULL); \
            if (UNEXPECTED(EG(exception))) { \
                zval_ptr_dtor(&bootstrap); \
                RETURN_FALSE; \
            } \
        } ZEND_HASH_FOREACH_END(); \
        zval_ptr_dtor(&bootstrap); \
    } \
}while (0)

extern zend_class_entry *syx_bootstrap_ce;

SYX_STARTUP_FUNCTION(bootstrap);
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
