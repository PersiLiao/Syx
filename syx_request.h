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

#ifndef SYX_REQUEST_H
#define SYX_REQUEST_H

#define SYX_REQUEST_PROPERTY_NAME_MODULE     "module"
#define SYX_REQUEST_PROPERTY_NAME_CONTROLLER "controller"
#define SYX_REQUEST_PROPERTY_NAME_ACTION     "action"
#define SYX_REQUEST_PROPERTY_NAME_METHOD     "method"
#define SYX_REQUEST_PROPERTY_NAME_PARAMS     "params"
#define SYX_REQUEST_PROPERTY_NAME_URI        "uri"
#define SYX_REQUEST_PROPERTY_NAME_STATE      "dispatched"
#define SYX_REQUEST_PROPERTY_NAME_LANG       "language"
#define SYX_REQUEST_PROPERTY_NAME_ROUTED     "routed"
#define SYX_REQUEST_PROPERTY_NAME_BASE       "_base_uri"
#define SYX_REQUEST_PROPERTY_NAME_EXCEPTION  "_exception"
#define SYX_REQUEST_PROPERTY_NAME_PROTOCOL   "_protocol"

#define SYX_REQUEST_SERVER_URI               "request_uri="

#define SYX_GLOBAL_VARS_TYPE                 unsigned int
#define SYX_GLOBAL_VARS_POST                 TRACK_VARS_POST
#define SYX_GLOBAL_VARS_GET                  TRACK_VARS_GET
#define SYX_GLOBAL_VARS_ENV                  TRACK_VARS_ENV
#define SYX_GLOBAL_VARS_FILES                TRACK_VARS_FILES
#define SYX_GLOBAL_VARS_SERVER               TRACK_VARS_SERVER
#define SYX_GLOBAL_VARS_REQUEST              TRACK_VARS_REQUEST
#define SYX_GLOBAL_VARS_COOKIE               TRACK_VARS_COOKIE

SYX_BEGIN_ARG_INFO_EX(syx_request_void_arginfo, 0, 0, 0)
SYX_END_ARG_INFO()

extern zend_class_entry *syx_request_ce;

void syx_request_params_init(syx_zval_t* this_ptr);
int syx_request_is_routed(syx_request_t *request);
int syx_request_is_dispatched(syx_request_t *request);
void syx_request_set_dispatched(syx_request_t *request, int flag);
void syx_request_set_routed(syx_request_t *request, int flag);
int syx_request_set_params_single(syx_request_t *instance, zend_string *key, zval *value);
int syx_request_set_params_multi(syx_request_t *instance, zval *values);

SYX_STARTUP_FUNCTION(request);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
