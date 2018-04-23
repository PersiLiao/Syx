PHP_ARG_ENABLE(syx, whether to enable syx support,
[  --enable-syx           Enable syx support])

AC_ARG_ENABLE(syx-debug,
[  --enable-syx-debug     Enable syx debug mode default=no],
[PHP_SYX_DEBUG=$enableval],
[PHP_SYX_DEBUG="no"])  

if test -z "$PHP_DEBUG" ; then
    AC_ARG_ENABLE(debug, [--enable-debug compile with debugging system], [PHP_DEBUG=$enableval],[PHP_DEBUG=no] )
fi

if test "$PHP_SYX" != "no"; then

  if test "$PHP_SYX_DEBUG" = "yes"; then
    AC_DEFINE(PHP_SYX_DEBUG,1,[define to 1 if you want to change the POST/GET by php script])
  else
    AC_DEFINE(PHP_SYX_DEBUG,0,[define to 1 if you want to change the POST/GET by php script])
  fi

  AC_MSG_CHECKING([PHP version])

  tmp_version=$PHP_VERSION
  if test -z "$tmp_version"; then
    if test -z "$PHP_CONFIG"; then
      AC_MSG_ERROR([php-config not found])
    fi
    php_version=`$PHP_CONFIG --version 2>/dev/null|head -n 1|sed -e 's#\([0-9]\.[0-9]*\.[0-9]*\)\(.*\)#\1#'`
  else
    php_version=`echo "$tmp_version"|sed -e 's#\([0-9]\.[0-9]*\.[0-9]*\)\(.*\)#\1#'`
  fi

  if test -z "$php_version"; then
    AC_MSG_ERROR([failed to detect PHP version, please report])
  fi

  ac_IFS=$IFS
  IFS="."
  set $php_version
  IFS=$ac_IFS
  syx_php_version=`expr [$]1 \* 1000000 + [$]2 \* 1000 + [$]3`

  if test "$syx_php_version" -le "5002000"; then
    AC_MSG_ERROR([You need at least PHP 5.2.0 to be able to use this version of Syx. PHP $php_version found])
  else
    AC_MSG_RESULT([$php_version, ok])
  fi
  PHP_NEW_EXTENSION(syx, 
    syx.c                           \
    syx_application.c               \
    syx_bootstrap.c                 \
    syx_dispatcher.c                \
    syx_exception.c                 \
    syx_config.c                    \
    configs/syx_config_ini.c        \
    configs/syx_config_simple.c     \
    syx_request.c                   \
    requests/syx_request_http.c     \
    requests/syx_request_simple.c   \
    requests/syx_request_swoole_tcp.c   \
    requests/syx_request_swoole_http.c   \
    requests/syx_request_swoole_websocket.c   \
    syx_response.c                  \
    responses/syx_response_http.c   \
    responses/syx_response_cli.c    \
    responses/syx_response_swoole.c   \
    responses/syx_response_swoole_tcp.c   \
    responses/syx_response_swoole_http.c   \
    responses/syx_response_swoole_websocket.c   \
    syx_view.c                      \
    views/syx_view_interface.c      \
    views/syx_view_simple.c         \
    syx_controller.c                \
    syx_action.c                    \
    syx_router.c                    \
    routes/syx_route_interface.c    \
    routes/syx_route_static.c       \
    routes/syx_route_simple.c       \
    routes/syx_route_supervar.c     \
    routes/syx_route_regex.c        \
    routes/syx_route_rewrite.c      \ 
    routes/syx_route_map.c          \
    syx_loader.c                    \
    syx_registry.c                  \
    syx_plugin.c                    \
    server/syx_server_interface.c   \
    server/syx_server_http.c        \
    server/syx_server_plugin.c      \
    server/syx_server_rpc.c         \
    server/syx_server_tcp.c         \
    server/syx_server_udp.c         \
    server/syx_server_websocket.c   \
    syx_server.c                    \
    library/console.c               \
    syx_command.c                   \
    syx_session.c,
  $ext_shared)
  PHP_ADD_BUILD_DIR([$ext_builddir/configs])
  PHP_ADD_BUILD_DIR([$ext_builddir/requests])
  PHP_ADD_BUILD_DIR([$ext_builddir/responses])
  PHP_ADD_BUILD_DIR([$ext_builddir/views])
  PHP_ADD_BUILD_DIR([$ext_builddir/routes])
  PHP_ADD_BUILD_DIR([$ext_builddir/server])
fi
