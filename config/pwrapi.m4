AC_DEFUN([PWR_CHECK_DEBUG],[

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

AC_DEFUN([PWR_CHECK_XML],[

    dnl this should really be a AC_ARG_WITH that points to the XML package

    AC_ARG_ENABLE(
        [xml],
        [AS_HELP_STRING([--disable-xml], 
            [Disable the use of XML for configuring the Power API])], 
        [], 
        [ enable_xml="yes" ]
    ) 
    
    AS_IF(
        [test "x$enable_xml" = "xyes"],
        [AC_DEFINE([USE_XML],[1], 
            [Set to 1 to use XML for configurating the Power API])]
    )

    AM_CONDITIONAL(USE_XML, [test "x$enable_xml" = xyes])
])

AC_DEFUN([PWR_CHECK_XMLRPC],[

    dnl this should really be a AC_ARG_WITH that points to the XMLRPC package
    
    AC_ARG_ENABLE(
        [xmlrpc],
        [AS_HELP_STRING([--enable-xmlrpc],
            [Enable the use of XML RPC for distribution of the Power API])]
    )

    AS_IF([test "x$enable_xmlrpc" = "xyes"], 
        [AC_DEFINE([USE_XMLRPC],[1], 
            [Set to 1 to use XML RPC for distribution of the Power API])]
    )
    
    AM_CONDITIONAL(USE_XMLRPC, [test "x$enable_xmlrpc" = xyes])
])

AM_CONDITIONAL(USE_PIDEV, [test "x$USE_PIDEV" = xyes])
AM_CONDITIONAL(USE_PGDEV, [test "x$USE_PGDEV" = xyes])
AM_CONDITIONAL(USE_STATIC, [test "x$USE_STATIC" = xyes])
