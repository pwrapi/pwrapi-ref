
AC_DEFUN([PWRAPI_CHECK_POWERGADGET], [
	AC_ARG_WITH([powergadget], [AS_HELP_STRING([--with-powergadget@<:@=DIR@:>@], [Use the powergadget binding in directory specified (DIR).])])

	pwrapi_check_powergadget_happy="yes"

	CPPFLAGS_saved="$CPPFLAGS"
  	LDFLAGS_saved="$LDFLAGS"
	LIBS_saved="$LIBS"
	POWERGADGET_LIBS=""

	AS_IF([test -z "$with_powergadget"],
		[POWERGADGET_CPPFLAGS=
                 POWERGADGET_LDFLAGS="-framework IntelPowerGadget"
                 POWERGADGET_LIBS=
                 LIBS="$LIBS $POWERGADGET_LIBS"],
		[	AS_IF([test "x$with_powergadget" = "xyes"],
				[POWERGADGET_CPPFLAGS=
                 POWERGADGET_LDFLAGS="-framework IntelPowerGadget"
           		 POWERGADGET_LIBS=
           		 LIBS="$LIBS $POWERGADGET_LIBS"],
				[POWERGADGET_CPPFLAGS="-I$with_powergadget/Headers"
                 CPPFLAGS="$POWERGADGET_CPPFLAGS $CPPFLAGS"
                 POWERGADGET_LDFLAGS="-framework IntelPowerGadget"
                 LDFLAGS="$POWERGADGET_LDFLAGS $LDFLAGS"
                 POWERGADGET_LIBS=
                 LIBS="$LIBS $POWERGADGET_LIBS"]
		)]
	)

	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

	AC_CHECK_HEADERS([IntelPowerGadgetLib.h], [], [pwrapi_check_powergadget_happy="no"])
	AC_LINK_IFELSE([AC_LANG_PROGRAM([], [
			int a;
		])], [pwrapi_check_powergadget_lib_happy="yes"],
		[pwrapi_check_powergadget_lib_happy="no"])

	AS_IF([test "x$pwrapi_check_powergadget_lib_happy" = "xno"],
		[pwrapi_check_powergadget_happy="no"])

	AC_LANG_RESTORE

	CPPFLAGS="$CPPFLAGS_saved"
	LDFLAGS="$LDFLAGS_saved"
	LIBS="$LIBS_saved"

	AC_SUBST([POWERGADGET_CPPFLAGS])
	AC_SUBST([POWERGADGET_LDFLAGS])
	AC_SUBST([POWERGADGET_LIBS])

	AM_CONDITIONAL([HAVE_POWERGADGET], [test "x$pwrapi_check_powergadget_happy" = "xyes"])
	AS_IF([test "x$pwrapi_check_powergadget_happy" = "xyes"], 
		[AC_DEFINE([HAVE_POWERGADGET], [1], [Set to 1 if powergadget is found during configuration])])

	AS_IF([test "x$pwrapi_check_powergadget_happy" = "xyes"], [$1], [$2])
])
