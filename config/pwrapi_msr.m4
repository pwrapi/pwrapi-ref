
AC_DEFUN([PWRAPI_CHECK_MSR], [
	AC_ARG_WITH([msr], [AS_HELP_STRING([--with-msr@<:@=DIR@:>@], [Use the msr binding in directory specified (DIR).])])

	pwrapi_check_msr_happy="yes"

	CPPFLAGS_saved="$CPPFLAGS"
  	LDFLAGS_saved="$LDFLAGS"
	LIBS_saved="$LIBS"
	MSR_LIBS=""

	AS_IF([test -z "$with_msr"],
		[MSR_CPPFLAGS=
                 MSR_LDFLAGS=
                 MSR_LIBS="-lmsr -lpthread"
                 LIBS="$LIBS $MSR_LIBS"],
		[	AS_IF([test "x$with_msr" = "xyes"],
				[MSR_CPPFLAGS=
           		 MSR_LDFLAGS=
           		 MSR_LIBS="-lmsr -lpthread"
           		 LIBS="$LIBS $MSR_LIBS"],
				[MSR_CPPFLAGS="-I$with_msr/include"
                 CPPFLAGS="$MSR_CPPFLAGS $CPPFLAGS"
                 MSR_LDFLAGS="-L$with_msr/lib"
                 LDFLAGS="$MSR_LDFLAGS $LDFLAGS"
                 MSR_LIBS="-lmsr -lpthread"
                 LIBS="$LIBS $MSR_LIBS"]
		)]
	)

	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS


	AC_LINK_IFELSE([AC_LANG_PROGRAM([], [
			int a;
		])], [pwrapi_check_msr_lib_happy="yes"],
		[pwrapi_check_msr_lib_happy="no"])

	AS_IF([test "x$pwrapi_check_msr_lib_happy" = "xno"],
		[pwrapi_check_msr_happy="no"])

	AC_LANG_RESTORE

	CPPFLAGS="$CPPFLAGS_saved"
	LDFLAGS="$LDFLAGS_saved"
	LIBS="$LIBS_saved"

	AC_SUBST([MSR_CPPFLAGS])
	AC_SUBST([MSR_LDFLAGS])
	AC_SUBST([MSR_LIBS])

	AM_CONDITIONAL([HAVE_MSR], [test "x$pwrapi_check_msr_happy" = "xyes"])
	AS_IF([test "x$pwrapi_check_msr_happy" = "xyes"],
		[AC_DEFINE([HAVE_MSR], [1], [Set to 1 if msr is found during configuration])])

	AS_IF([test "x$pwrapi_check_msr_happy" = "xyes"], [$1], [$2])
])
