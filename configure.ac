orig_CFLAGS="$CFLAGS"

AC_INIT([pwrapi], [1.00])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([src/config.h])
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

SWIG_ENABLE_CXX
SWIG_PYTHON


m4_include([config/pwrapi.m4])

PWR_CHECK_SWIG()

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/pwr/Makefile
                 src/bindings/python/Makefile
                 src/plugins/Makefile
                 tools/Makefile
                 examples/Makefile])

AC_OUTPUT

