# dnl function for enabling/disabling udns support
AC_DEFUN([TORRENT_WITH_UDNS], [
  AC_ARG_ENABLE(udns,
    AC_HELP_STRING([--enable-udns], [use udns instead of synchronous DNS resolution [[default=check]]]),
    [
      if test "$enableval" = "yes"; then
        AC_CHECK_HEADERS([udns.h], [have_udns=yes], [have_udns=no])

        if test "$have_udns" = "yes"; then
          AC_DEFINE(USE_UDNS, 1, Define to build with udns support.)
          LIBS="$LIBS -ludns"

        else
          AC_MSG_ERROR([udns requested but not found])
        fi
      fi
    ])
])
