AC_INIT(rtorrent, 0.9.8, sundell.software@gmail.com)

AC_DEFINE(API_VERSION, 10, api version)
AC_DEFINE(STICKZ_VERSION, "0.9.8-v7.2", rtorrent stickz version)

AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_HEADERS(config.h)

AC_PROG_RANLIB
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_SYS_LARGEFILE

AX_CXX_COMPILE_STDCXX(17, noext, mandatory)

RAK_CHECK_CFLAGS
RAK_CHECK_CXXFLAGS
RAK_ENABLE_DEBUG
RAK_ENABLE_EXTRA_DEBUG
RAK_ENABLE_WERROR

TORRENT_DISABLE_IPV6

TORRENT_CHECK_EXECINFO

TORRENT_ENABLE_ARCH
TORRENT_WITH_SYSROOT

TORRENT_WITHOUT_VARIABLE_FDSET
TORRENT_WITHOUT_STATVFS
TORRENT_WITHOUT_STATFS

AX_PTHREAD([], AC_MSG_ERROR([requires pthread]))
AX_WITH_CURSES

if test "x$ax_cv_ncursesw" != xyes && test "x$ax_cv_ncurses" != xyes; then
	AC_MSG_ERROR([requires either NcursesW or Ncurses library])
fi

PKG_CHECK_MODULES([LIBCURL], [libcurl],, [LIBCURL_CHECK_CONFIG])
PKG_CHECK_MODULES([CPPUNIT], [cppunit],, [no_cppunit="yes"])
PKG_CHECK_MODULES([DEPENDENCIES], [libtorrent >= 0.13.8],, [no_libtorrent="yes"])

AC_LANG_PUSH(C++)
TORRENT_WITH_XMLRPC_C
AC_LANG_POP(C++)

TORRENT_WITH_TINYXML2

if test ${with_xmlrpc_c+y} && test ${with_xmlrpc_tinyxml2+y}; then
  AC_MSG_ERROR([--with-xmlrpc-c and --with-xmlrpc-tinyxml2 cannot be used together. Please choose only one])
fi

AC_DEFINE(HAVE_CONFIG_H, 1, true if config.h was included)
AC_DEFINE(USER_AGENT, [std::string(PACKAGE "/" VERSION "/") + torrent::version()], Http user agent)

AC_CHECK_FUNCS(posix_memalign)
TORRENT_CHECK_CACHELINE()
TORRENT_CHECK_POPCOUNT()

CC_ATTRIBUTE_UNUSED(
	AC_DEFINE([__UNUSED], [__attribute__((unused))], [Wrapper around unused attribute]),
	AC_DEFINE([__UNUSED], [], [Null-wrapper if unused attribute is unsupported])
)

dnl Only update global build variables immediately before generating the output,
dnl to avoid affecting the global build environment for other autoconf checks.
LIBS="$PTHREAD_LIBS $CURSES_LIB $CURSES_LIBS $LIBCURL $LIBCURL_LIBS $DEPENDENCIES_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS $LIBCURL_CPPFLAGS $LIBCURL_CFLAGS $DEPENDENCIES_CFLAGS $CURSES_CFLAGS"
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS $LIBCURL_CPPFLAGS $LIBCURL_CFLAGS $DEPENDENCIES_CFLAGS $CURSES_CFLAGS"

AC_OUTPUT([
	Makefile
	doc/Makefile
	src/Makefile
	src/core/Makefile
	src/display/Makefile
	src/input/Makefile
	src/rpc/Makefile
	src/ui/Makefile
	src/utils/Makefile
        test/Makefile
])
