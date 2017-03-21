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

#ifndef SYX_CONTROLLER_H
#define SYX_CONTROLLER_H

#define SYX_CONTROLLER_PROPERTY_NAME_MODULE		"_module"
#define SYX_CONTROLLER_PROPERTY_NAME_NAME		"_name"
#define SYX_CONTROLLER_PROPERTY_NAME_SCRIPT		"_script_path"
#define SYX_CONTROLLER_PROPERTY_NAME_RESPONSE	"_response"
#define SYX_CONTROLLER_PROPERTY_NAME_REQUEST	"_request"
#define SYX_CONTROLLER_PROPERTY_NAME_ARGS		"_invoke_args"
#define SYX_CONTROLLER_PROPERTY_NAME_ACTIONS	"actions"
#define SYX_CONTROLLER_PROPERTY_NAME_VIEW		"_view"

#define SYX_CONTROLLER_PROPERTY_NAME_RENDER     "syxAutoRender"

extern zend_class_entry *syx_controller_ce;
int syx_controller_construct(zend_class_entry *ce, syx_controller_t *self,
		syx_request_t *request, syx_response_t *response, syx_view_t *view, zval *args);
zend_string * syx_controller_render(zval *instance, char *action_name, int len, zval *var_array);
int syx_controller_display(zval *instance, char *action_name, int len, zval *var_array);
SYX_STARTUP_FUNCTION(controller);
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

