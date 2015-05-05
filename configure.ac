orig_CFLAGS="$CFLAGS"

AC_INIT([powapi], [0.01])
AC_CONFIG_SRCDIR([pow.h])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_CXX
dnl Supply default CFLAGS, if not specified
if test -z "$orig_CFLAGS"; then
  if test x$GCC = xyes; then
    CFLAGS="-g -O3"
  fi
fi

AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_CONFIG_MACRO_DIR([m4])
m4_include([powapi.m4])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 test/Makefile])
AC_OUTPUT
