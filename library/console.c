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

#include "php_syx.h"
#include "console.h"

static inline zend_string* syx_console_string(char *message, char* fore_color, char *back_color){
    zend_string *syx_str = strpprintf(0, "%s%s%s%s%s%s%s", "\033[", fore_color, "m", message, "\033[", back_color, "m\033[0m");
    return syx_str;
}

void syx_console_output(char *message, int type){
    zend_string *syx_str = NULL;
    switch(type){
        case SYX_CONSOLE_TYPE_NOTICE:{
            syx_str = syx_console_string(message, SYX_CONSOLE_TYPE_NOTICE_FORE_COLOR, SYX_CONSOLE_TYPE_NOTICE_BACK_COLOR);
            fprintf(stdout, "%s", syx_str->val);
            break;
        }
        case SYX_CONSOLE_TYPE_WARN:{
            syx_str = syx_console_string(message, SYX_CONSOLE_TYPE_WARN_FORE_COLOR, SYX_CONSOLE_TYPE_WARN_BACK_COLOR);
            fprintf(stdout, "%s", syx_str->val);
            break;
        }
        case SYX_CONSOLE_TYPE_ERROR:{
            syx_str = syx_console_string(message, SYX_CONSOLE_TYPE_ERROR_FORE_COLOR, SYX_CONSOLE_TYPE_ERROR_BACK_COLOR);
            fprintf(stdout, "%s", syx_str->val);
            break;
        }
        case SYX_CONSOLE_TYPE_SUCCESS:{
            syx_str = syx_console_string(message, SYX_CONSOLE_TYPE_SUCCESS_FORE_COLOR, SYX_CONSOLE_TYPE_SUCCESS_BACK_COLOR);
            fprintf(stdout, "%s", syx_str->val);
            break;
        }
        default:{
            fprintf(stdout, message);
        }
    }
    if(syx_str != NULL){
        zval_ptr_dtor(syx_str);
    }
}
