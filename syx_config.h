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

#ifndef SYX_CONFIG_H
#define SYX_CONFIG_H

#define SYX_EXTRACT_FUNC_NAME            "extract"
#define SYX_CONFIG_PROPERT_NAME          "_config"
#define SYX_CONFIG_PROPERT_NAME_READONLY "_readonly"

struct _syx_config_cache {
	long ctime;
	HashTable *data;
};

typedef struct _syx_config_cache syx_config_cache;

extern zend_class_entry *syx_config_ce;

syx_config_t *syx_config_instance(syx_config_t *this_ptr, zval *arg1, zval *arg2 TSRMLS_DC);
void syx_config_unserialize(syx_config_t *self, HashTable *data TSRMLS_DC);

SYX_STARTUP_FUNCTION(config);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
