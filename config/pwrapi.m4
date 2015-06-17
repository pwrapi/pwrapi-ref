AC_DEFUN([PWRAPI_CHECK_DEBUG],[

    AC_ARG_ENABLE(
        [debug],
        [AS_HELP_STRING([--enable-debug], 
            [Enable debug features of the Power API])]
    )

    AS_IF(
        [test "x$enable_debug" = "xyes"],
        [AC_DEFINE([USE_DEBUG],[1], 
            [Set to 1 to use debugging features of the Power API])]    
    ) 
])

AM_CONDITIONAL(USE_STATIC, [test "x$USE_STATIC" = xyes])
