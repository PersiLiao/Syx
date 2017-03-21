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

#ifndef SYX_NAMESPACE_H
#define SYX_NAMESPACE_H

#define SYX_BEGIN_ARG_INFO		ZEND_BEGIN_ARG_INFO
#define SYX_BEGIN_ARG_INFO_EX	ZEND_BEGIN_ARG_INFO_EX

#define SYX_INIT_CLASS_ENTRY(ce, name_ns, methods) \
	INIT_CLASS_ENTRY(ce, name_ns, methods);

#define SYX_END_ARG_INFO		ZEND_END_ARG_INFO
#define SYX_ARG_INFO			ZEND_ARG_INFO
#define SYX_ARG_OBJ_INFO 	ZEND_ARG_OBJ_INFO
#define SYX_ARG_ARRAY_INFO 	ZEND_ARG_ARRAY_INFO

#endif	/* PHP_SYX_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
