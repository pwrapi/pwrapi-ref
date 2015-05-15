orig_CFLAGS="$CFLAGS"

AC_INIT([pwrapi], [1.00])
AC_CONFIG_SRCDIR([pwr/pwr.h])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
dnl Supply default CFLAGS, if not specified
if test -z "$orig_CFLAGS"; then
  if test x$GCC = xyes; then
    CFLAGS="-g -O3"
  fi
fi

AC_PROG_INSTALL
AC_PROG_LIBTOOL

AM_PATH_PYTHON(2.3)
SWIG_ENABLE_CXX
SWIG_PYTHON

AC_CONFIG_MACRO_DIR([m4])
m4_include([pwrapi.m4])

AC_CONFIG_FILES([Makefile
                 pwr/Makefile
                 plugins/Makefile
                 tools/Makefile
                 examples/Makefile])
AC_OUTPUT

