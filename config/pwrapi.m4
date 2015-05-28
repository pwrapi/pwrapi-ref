AC_DEFINE(DEBUG,"n","Debug flag is not set")

AM_CONDITIONAL(BUILD_DAEMON, [test x"$DAEMON" = "y"])
AM_CONDITIONAL(BUILD_DEBUG, [test x"$DEBUG" = "y"])
AM_CONDITIONAL(BUILD_XML, [test x"$XML" = "y"])
AM_CONDITIONAL(BUILD_XMLRPC, [test x"$XMLRPC" = "y"])
AM_CONDITIONAL(BUILD_PIDEV, [test x"$PIDEV" = "y"])
AM_CONDITIONAL(BUILD_MACOS, [test x"$MACOS" = "y"])
AM_CONDITIONAL(BUILD_STATIC, [test x"$STATIC" = "y"])

AC_DEFUN([PWR_CHECK_SWIG],[
	AC_CHECK_PROG([swig],[swig],[yes],[no])
	AM_CONDITIONAL([HAVE_SWIG], [test "x$swig" = xyes])
])
