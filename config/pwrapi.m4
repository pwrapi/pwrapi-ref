AC_DEFUN([PWR_CHECK_DEBUG],[
    use_debug="no"
    AC_ARG_ENABLE([debug],[AS_HELP_STRING([--enable-debug],
        [Enable debug features of the Power API])])
    AS_IF([test "x$DEBUG" = "xyes"],[use_debug = "yes"])
    AS_IF([test "$use_debug" = "yes"], [AC_DEFINE([USE_DEBUG],[1],
        [Set to 1 to use debugging features of the Power API])])
])

AC_DEFUN([PWR_CHECK_XML],[
    use_xml="yes"
    AC_ARG_ENABLE([xml],[AS_HELP_STRING([--disable-xml],
        [Disable the use of XML for configuring the Power API])])
    AS_IF([test "x$XML" = "xno"],[use_xml = "no"])
    AS_IF([test "$use_xml" = "yes"], [AC_DEFINE([USE_XML],[1],
        [Set to 1 to use XML for configurating the Power API])])
])

AC_DEFUN([PWR_CHECK_XMLRPC],[
    use_xmlrpc="yes"
    AC_ARG_ENABLE([xmlrpc],[AS_HELP_STRING([--disable-xmlrpc],
        [Disable the use of XML RPC for distribution of the Power API])])
    AS_IF([test "x$XMLRPC" = "xno"],[use_xmlrpc = "no"])
    AS_IF([test "$use_xmlrpc" = "yes"], [AC_DEFINE([USE_XMLRPC],[1],
        [Set to 1 to use XML RPC for distribution of the Power API])])
])

AM_CONDITIONAL(USE_XML, [test "x$USE_XML" = xyes])
AM_CONDITIONAL(USE_XMLRPC, [test "x$USE_XMLRPC" = xyes])
AM_CONDITIONAL(USE_PIDEV, [test "x$USE_PIDEV" = xyes])
AM_CONDITIONAL(USE_PGDEV, [test "x$USE_PGDEV" = xyes])
AM_CONDITIONAL(USE_STATIC, [test "x$USE_STATIC" = xyes])

AC_DEFUN([PWR_CHECK_SWIG],[
	AC_CHECK_PROG([swig],[swig],[yes],[no])
	AM_CONDITIONAL([HAVE_SWIG], [test "x$swig" = xyes])
])
