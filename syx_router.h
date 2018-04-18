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

#ifndef SYX_ROUTER_H
#define SYX_ROUTER_H

#define SYX_ROUTER_DEFAULT_ACTION	 	"index"
#define SYX_ROUTER_DEFAULT_CONTROLLER  	"Index"
#define SYX_ROUTER_DEFAULT_MODULE	  	"index"
#define SYX_DEFAULT_EXT 		 	   	"php"

#define SYX_ROUTER_PROPERTY_NAME_ROUTES 		"_routes"
#define SYX_ROUTER_PROPERTY_NAME_CURRENT_ROUTE	"_current"

extern zend_class_entry *syx_router_ce;

syx_router_t *syx_router_instance(syx_router_t *this_ptr);
void syx_router_parse_parameters(char *uri, zval *params);
int syx_router_route(syx_router_t *router, syx_request_t *request);

SYX_STARTUP_FUNCTION(router);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
