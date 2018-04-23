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

#ifndef SYX_COMMAND_H_
#define SYX_COMMAND_H_

#define SYX_COMMAND_PREFIX_OPERATORS "-"
#define SYX_COMMAND_ASSIGN_OPERATORS "="

#define SYX_COMMAND_ACTION_START      1
#define SYX_COMMAND_ACTION_RELOAD     2
#define SYX_COMMAND_ACTION_RELOAD_ALL 3
#define SYX_COMMAND_ACTION_RESTART    4
#define SYX_COMMAND_ACTION_STOP       5

#define SYX_COMMAND_OPT_CHECK(pzval, opt_len, str) (strncasecmp(Z_STRVAL_P(pzval) + opt_len, (str), strlen(str)) == 0)

typedef int (*syx_command_handler)(syx_server_conf *serv_conf, syx_application_conf *app_conf, syx_zval_t *val, char *syx_app_cwd);

typedef struct _syx_opt_struct {
    char *opt_char;
    unsigned int opt_len;
    int  need_param;
    char *opt_name;
    syx_command_handler handler;
} syx_opt_struct;

void syx_command_parse();

#endif /* COMMAND_H_ */
