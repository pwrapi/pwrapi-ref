
AC_DEFUN([PWRAPI_CHECK_PIAPI], [
	AC_ARG_WITH([piapi], [AS_HELP_STRING([--with-piapi@<:@=DIR@:>@], [Use the piapi binding in directory specified (DIR).])])

	pwrapi_check_piapi_happy="yes"

	CPPFLAGS_saved="$CPPFLAGS"
  	LDFLAGS_saved="$LDFLAGS"
	LIBS_saved="$LIBS"
	PIAPI_LIBS=""

	AS_IF([test -z "$with_piapi"],
		[PIAPI_CPPFLAGS=
                 PIAPI_LDFLAGS=
                 PIAPI_LIBS="-lpiapi -lpthread"
                 LIBS="$LIBS $PIAPI_LIBS"],
		[	AS_IF([test "x$with_piapi" = "xyes"],
				[PIAPI_CPPFLAGS=
           		 PIAPI_LDFLAGS=
           		 PIAPI_LIBS="-lpiapi -lpthread"
           		 LIBS="$LIBS $PIAPI_LIBS"],
				[PIAPI_CPPFLAGS="-I$with_piapi/include"
                 CPPFLAGS="$PIAPI_CPPFLAGS $CPPFLAGS"
                 PIAPI_LDFLAGS="-L$with_piapi/lib"
                 LDFLAGS="$PIAPI_LDFLAGS $LDFLAGS"
                 PIAPI_LIBS="-lpiapi -lpthread"
                 LIBS="$LIBS $PIAPI_LIBS"]
		)]
	)

	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

	AC_CHECK_HEADERS([piapi.h], [], [pwrapi_check_piapi_happy="no"])
	AC_LINK_IFELSE([AC_LANG_PROGRAM([], [
			int a;
		])], [pwrapi_check_piapi_lib_happy="yes"],
		[pwrapi_check_piapi_lib_happy="no"])

	AS_IF([test "x$pwrapi_check_piapi_lib_happy" = "xno"],
		[pwrapi_check_piapi_happy="no"])

	AC_LANG_RESTORE

	CPPFLAGS="$CPPFLAGS_saved"
	LDFLAGS="$LDFLAGS_saved"
	LIBS="$LIBS_saved"

	AC_SUBST([PIAPI_CPPFLAGS])
	AC_SUBST([PIAPI_LDFLAGS])
	AC_SUBST([PIAPI_LIBS])

	AM_CONDITIONAL([HAVE_PIAPI], [test "x$pwrapi_check_piapi_happy" = "xyes"])
	AS_IF([test "x$pwrapi_check_piapi_happy" = "xyes"], 
		[AC_DEFINE([HAVE_PIAPI], [1], [Set to 1 if piapi is found during configuration])])

	AS_IF([test "x$pwrapi_check_piapi_happy" = "xyes"], [$1], [$2])
])
