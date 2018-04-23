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

#ifndef SYX_CONSOLE_H_
#define SYX_CONSOLE_H_

#define SYX_CONSOLE_TYPE_SIMPLE   0
#define SYX_CONSOLE_TYPE_NOTICE   1
#define SYX_CONSOLE_TYPE_NOTICE_FORE_COLOR "0;37"
#define SYX_CONSOLE_TYPE_NOTICE_BACK_COLOR "47"
#define SYX_CONSOLE_TYPE_WARN     2
#define SYX_CONSOLE_TYPE_WARN_FORE_COLOR "1;33"
#define SYX_CONSOLE_TYPE_WARN_BACK_COLOR "43"
#define SYX_CONSOLE_TYPE_ERROR    3
#define SYX_CONSOLE_TYPE_ERROR_FORE_COLOR "0;31"
#define SYX_CONSOLE_TYPE_ERROR_BACK_COLOR "41"
#define SYX_CONSOLE_TYPE_SUCCESS  4
#define SYX_CONSOLE_TYPE_SUCCESS_FORE_COLOR "0;32"
#define SYX_CONSOLE_TYPE_SUCCESS_BACK_COLOR "42"

void syx_console_output(char *message, int type);

#endif /* CONSOLE_H_ */
