
AC_DEFUN([PWRAPI_CHECK_XMLRPC_C], [
	AC_ARG_WITH([xmlrpc_c], [AS_HELP_STRING([--with-xmlrpc-c@<:@=DIR@:>@], [Use the xmlrpc-c lib for C in directory specified (DIR).])])

	pwrapi_check_xmlrpc_happy="yes"

	CPPFLAGS_saved="$CPPFLAGS"
  	LDFLAGS_saved="$LDFLAGS"
	LIBS_saved="$LIBS"
	XMLRPC_C_LIBS=""

	AS_IF([test -z "$with_xmlrpc_c"],
		[XMLRPC_C_CPPFLAGS=
                 XMLRPC_C_LDFLAGS=
                 XMLRPC_C_LIBS="-lxmlrpc_client -lxmlrpc  -lxmlrpc_util"
                 LIBS="$LIBS $XMLRPC_C_LIBS"],
		[	AS_IF([test "x$with_xmlrpc_c" = "xyes"],
				[XMLRPC_C_CPPFLAGS=
                 		 XMLRPC_C_LDFLAGS=
                 		 XMLRPC_C_LIBS="-lxmlrpc_client -lxmlrpc  -lxmlrpc_util"
                 		 LIBS="$LIBS $XMLRPC_C_LIBS"],
				[XMLRPC_C_CPPFLAGS="-I$with_xmlrpc_c/include"
                 		 CPPFLAGS="$XMLRPC_C_CPPFLAGS $CPPFLAGS"
                 		 XMLRPC_C_LDFLAGS="-L$with_xmlrpc_c/lib"
                 		 LDFLAGS="$XMLRPC_C_LDFLAGS $LDFLAGS"
                 		 XMLRPC_C_LIBS="-lxmlrpc_client -lxmlrpc  -lxmlrpc_util"
                 		 LIBS="$LIBS $XMLRPC_C_LIBS"]
		)]
	)

	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

	AC_CHECK_HEADERS([xmlrpc-c/client.h], [], [pwrapi_check_xmlrpc_happy="no"])
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

	AC_SUBST([XMLRPC_C_CPPFLAGS])
	AC_SUBST([XMLRPC_C_LDFLAGS])
	AC_SUBST([XMLRPC_C_LIBS])

	AM_CONDITIONAL([HAVE_XMLRPC_C], [test "x$pwrapi_check_xmlrpc_happy" = "xyes"])
	AS_IF([test "x$pwrapi_check_xmlrpc_happy" = "xyes"], 
		[AC_DEFINE([HAVE_XMLRPC_C], [1], [Set to 1 if xmlrpc-c is found during configuration])])

	AS_IF([test "x$pwrapi_check_xmlrpc_happy" = "xyes"], [$1], [$2])
])
