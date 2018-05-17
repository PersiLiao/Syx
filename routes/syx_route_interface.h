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

#ifndef SYX_ROUTER_INTERFACE_H
#define SYX_ROUTER_INTERFACE_H

#define SYX_ROUTE_ASSEMBLE_MOUDLE_FORMAT ":m"
#define SYX_ROUTE_ASSEMBLE_ACTION_FORMAT ":a"
#define SYX_ROUTE_ASSEMBLE_CONTROLLER_FORMAT ":c"

#define SYX_ROUTE_PROPETY_NAME_MATCH  	"_route"
#define SYX_ROUTE_PROPETY_NAME_ROUTE  	"_default"
#define SYX_ROUTE_PROPETY_NAME_MAP	 	"_maps"
#define SYX_ROUTE_PROPETY_NAME_VERIFY 	"_verify"
#define SYX_ROUTE_PROPETY_NAME_REVERSE 	"_reverse"

#define SYX_ROUTER_URL_DELIMIETER  	 "/"
#define SYX_ROUTE_REGEX_DILIMITER  	 '#'

SYX_BEGIN_ARG_INFO_EX(syx_route_route_arginfo, 0, 0, 1)
	SYX_ARG_OBJ_INFO(0, request, Syx\\RequestAbstract, 0)
SYX_END_ARG_INFO()

SYX_BEGIN_ARG_INFO_EX(syx_route_assemble_arginfo, 0, 0, 1)
	SYX_ARG_ARRAY_INFO(0, info, 0)
	SYX_ARG_ARRAY_INFO(0, query, 1)
SYX_END_ARG_INFO()


extern zend_class_entry *syx_route_ce;

syx_route_t * syx_route_instance(syx_route_t *this_ptr,  zval *config);

SYX_STARTUP_FUNCTION(route);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
