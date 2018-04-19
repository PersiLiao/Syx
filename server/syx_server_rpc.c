/*
  +----------------------------------------------------------------------+
  | Syx Framework                                                        |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: XiangChu Liao  <ican.lxc@gmail.com>                          |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "Zend/zend_interfaces.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_config.h"
#include "syx_loader.h"
#include "syx_application.h"
#include "syx_bootstrap.h"
#include "syx_exception.h"

#include "syx_server.h"
#include "server/syx_server_interface.h"
#include "server/syx_server_plugin.h"
#include "server/syx_server_tcp.h"
#include "server/syx_server_http.h"
#include "server/syx_server_rpc.h"