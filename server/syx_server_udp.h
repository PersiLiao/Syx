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

#ifndef SYX_SERVER_UDP_H_
#define SYX_SERVER_UDP_H_

void syx_server_swoole_onPacket(syx_server_t* syx_swoole_server_o);

extern zend_class_entry *syx_server_udp_ce;

SYX_STARTUP_FUNCTION(server_udp);

#endif /* SYX_SERVER_UDP_H_ */
