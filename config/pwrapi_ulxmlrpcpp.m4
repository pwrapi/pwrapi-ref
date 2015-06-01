
AC_DEFUN([PWRAPI_CHECK_ULXMLRPCPP], [
	AC_ARG_WITH([ulxmlrpcpp], [AS_HELP_STRING([--with-ulxmlrpcpp@<:@=DIR@:>@], [Use the ultra lightweight xml-rpc lib for C++ in directory specified (DIR).])])

	pwrapi_check_xmlrpc_happy="yes"

	CPPFLAGS_saved="$CPPFLAGS"
  	LDFLAGS_saved="$LDFLAGS"
	LIBS_saved="$LIBS"
	XMLRPC_LIBS=""

	AS_IF([test -z "$with_ulxmlrpcpp"],
		[XMLRPC_CPPFLAGS=
                 XMLRPC_LDFLAGS=
                 XMLRPC_LIBS="-lulxmlrpcpp"
                 LIBS="$LIBS $XMLRPC_LIBS"],
		[	AS_IF([test "x$with_ulxmlrpcpp" = "xyes"],
				[XMLRPC_CPPFLAGS=
                 		 XMLRPC_LDFLAGS=
                 		 XMLRPC_LIBS="-lulxmlrpcpp"
                 		 LIBS="$LIBS $XMLRPC_LIBS"],
				[XMLRPC_CPPFLAGS="-I$with_ulxmlrpcpp/include"
                 		 CPPFLAGS="$XMLRPC_CPPFLAGS $CPPFLAGS"
                 		 XMLRPC_LDFLAGS="-L$with_ulxmlrpcpp/lib"
                 		 LDFLAGS="$XMLRPC_LDFLAGS $LDFLAGS"
                 		 XMLRPC_LIBS="-lulxmlrpcpp"
                 		 LIBS="$LIBS $XMLRPC_LIBS"]
		)]
	)

	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

	AC_CHECK_HEADERS([ulxmlrpcpp/ulxmlrpcpp.h], [], [pwrapi_check_xmlrpc_happy="no"])
	AC_LINK_IFELSE([AC_LANG_PROGRAM([], [
			int a;
		])], [pwrapi_check_xmlrpc_lib_happy="yes"],
	        [pwrapi_check_xmlrpc_lib_happy="no"])

	AS_IF([test "x$pwrapi_check_xmlrpc_lib_happy" = "xno"],
		[pwrapi_check_xmlrpc_happy="no"])

	AC_LANG_RESTORE

	CPPFLAGS="$CPPFLAGS_saved"
	LDFLAGS="$LDFLAGS_saved"
	LIBS="$LIBS_saved"

	AC_SUBST([XMLRPC_CPPFLAGS])
	AC_SUBST([XMLRPC_LDFLAGS])
	AC_SUBST([XMLRPC_LIBS])

	AM_CONDITIONAL([HAVE_XMLRPC], [test "x$pwrapi_check_xmlrpc_happy" = "xyes"])
	AS_IF([test "x$pwrapi_check_xmlrpc_happy" = "xyes"], 
		[AC_DEFINE([HAVE_XMLRPC], [1], [Set to 1 if xmlrpc is found during configuration])])

	AS_IF([test "x$pwrapi_check_xmlrpc_happy" = "xyes"], [$1], [$2])
])
