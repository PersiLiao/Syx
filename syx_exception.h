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

#ifndef SYX_EXCEPTION_H
#define SYX_EXCEPTION_H

#define SYX_MAX_BUILDIN_EXCEPTION	10

#define SYX_ERR_BASE 				512
#define SYX_UERR_BASE				1024
#define SYX_ERR_MASK				127

#define SYX_ERR_STARTUP_FAILED 		512
#define SYX_ERR_ROUTE_FAILED 		513
#define SYX_ERR_DISPATCH_FAILED 	514
#define SYX_ERR_NOTFOUND_MODULE 	515
#define SYX_ERR_NOTFOUND_CONTROLLER 516
#define SYX_ERR_NOTFOUND_ACTION 	517
#define SYX_ERR_NOTFOUND_VIEW 		518
#define SYX_ERR_CALL_FAILED			519
#define SYX_ERR_AUTOLOAD_FAILED 	520
#define SYX_ERR_TYPE_ERROR			521
#define SYX_ERR_RESPONSE_ERROR      522

#define SYX_EXCEPTION_OFFSET(x) (x & SYX_ERR_MASK)

#define SYX_CORRESPOND_ERROR(x) (x>>9L)

#define SYX_EXCEPTION_HANDLE(dispatcher, request, response) \
	if (EG(exception)) { \
		if (SYX_G(catch_exception) \
				 && instanceof_function(EG(exception)->ce, zend_exception_get_default())) { \
			syx_dispatcher_exception_handler(dispatcher, request, response); \
		} \
		zval_ptr_dtor(response); \
		return NULL; \
	}

#define SYX_EXCEPTION_HANDLE_NORET(dispatcher, request, response) \
	if (EG(exception)) { \
		if (SYX_G(catch_exception) \
	   			&& instanceof_function(EG(exception)->ce, zend_exception_get_default())) { \
			syx_dispatcher_exception_handler(dispatcher, request, response); \
		} \
	}

#define SYX_EXCEPTION_ERASE_EXCEPTION() \
	do { \
		EG(current_execute_data)->opline = EG(opline_before_exception); \
	} while(0)

extern zend_class_entry *syx_ce_RuntimeException;
extern zend_class_entry *syx_exception_ce;
extern zend_class_entry *syx_buildin_exceptions[SYX_MAX_BUILDIN_EXCEPTION];
void syx_trigger_error(int type, char *format, ...);
void syx_throw_exception(long code, char *message);

SYX_STARTUP_FUNCTION(exception);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
