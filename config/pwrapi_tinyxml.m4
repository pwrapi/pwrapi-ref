
AC_DEFUN([PWRAPI_CHECK_TINYXML], [
	AC_ARG_WITH([tinyxml], [AS_HELP_STRING([--with-tinyxml@<:@=DIR@:>@], [Use the tinyxml binding in directory specified (DIR).])])

	pwrapi_check_tinyxml_happy="yes"

	CPPFLAGS_saved="$CPPFLAGS"
  	LDFLAGS_saved="$LDFLAGS"
	LIBS_saved="$LIBS"
	TINYXML_LIBS=""

	AS_IF([test -z "$with_tinyxml"],
		[TINYXML_CPPFLAGS=
                 TINYXML_LDFLAGS=
                 TINYXML_LIBS="-ltinyxml2"
                 LIBS="$LIBS $TINYXML_LIBS"],
		[	AS_IF([test "x$with_tinyxml" = "xyes"],
				[TINYXML_CPPFLAGS=
                 		 TINYXML_LDFLAGS=
                 		 TINYXML_LIBS="-ltinyxml2"
                 		 LIBS="$LIBS $TINYXML_LIBS"],
				[TINYXML_CPPFLAGS="-I$with_tinyxml/include"
                 		 CPPFLAGS="$TINYXML_CPPFLAGS $CPPFLAGS"
                 		 TINYXML_LDFLAGS="-L$with_tinyxml/lib"
                 		 LDFLAGS="$TINYXML_LDFLAGS $LDFLAGS"
                 		 TINYXML_LIBS="-ltinyxml2"
                 		 LIBS="$LIBS $TINYXML_LIBS"]
		)]
	)

	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

	AC_CHECK_HEADERS([tinyxml2.h], [], [pwrapi_check_tinyxml_happy="no"])
	AC_LINK_IFELSE([AC_LANG_PROGRAM([], [
			int a;
		])], [pwrapi_check_tinyxml_lib_happy="yes"],
		[pwrapi_check_tinyxml_lib_happy="no"])

	AS_IF([test "x$pwrapi_check_tinyxml_lib_happy" = "xno"],
		[pwrapi_check_tinyxml_happy="no"])

	AC_LANG_RESTORE

	CPPFLAGS="$CPPFLAGS_saved"
	LDFLAGS="$LDFLAGS_saved"
	LIBS="$LIBS_saved"

	AC_SUBST([TINYXML_CPPFLAGS])
	AC_SUBST([TINYXML_LDFLAGS])
	AC_SUBST([TINYXML_LIBS])

	AM_CONDITIONAL([HAVE_TINYXML], [test "x$pwrapi_check_tinyxml_happy" = "xyes"])
	AS_IF([test "x$pwrapi_check_tinyxml_happy" = "xyes"], 
		[AC_DEFINE([HAVE_TINYXML], [1], [Set to 1 if tinyxml is found during configuration])])

	AS_IF([test "x$pwrapi_check_tinyxml_happy" = "xyes"], [$1], [$2])
])
