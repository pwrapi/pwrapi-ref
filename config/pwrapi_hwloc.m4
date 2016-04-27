
AC_DEFUN([PWRAPI_CHECK_HWLOC], [
	AC_ARG_WITH([hwloc], [AS_HELP_STRING([--with-hwloc@<:@=DIR@:>@], [Use the hwloc binding in directory specified (DIR).])])

	pwrapi_check_hwloc_happy="yes"

	CPPFLAGS_saved="$CPPFLAGS"
  	LDFLAGS_saved="$LDFLAGS"
	LIBS_saved="$LIBS"
	HWLOC_LIBS=""

	AS_IF([test -z "$with_hwloc"],
		[HWLOC_CPPFLAGS=
                 HWLOC_LDFLAGS=
                 HWLOC_LIBS="-lhwloc"
                 LIBS="$LIBS $HWLOC_LIBS"],
		[	AS_IF([test "x$with_hwloc" = "xyes"],
				[HWLOC_CPPFLAGS=
           		 HWLOC_LDFLAGS=
           		 HWLOC_LIBS="-lhwloc"
           		 LIBS="$LIBS $HWLOC_LIBS"],
				[HWLOC_CPPFLAGS="-I$with_hwloc/include"
                 CPPFLAGS="$HWLOC_CPPFLAGS $CPPFLAGS"
                 HWLOC_LDFLAGS="-L$with_hwloc/lib"
                 LDFLAGS="$HWLOC_LDFLAGS $LDFLAGS"
                 HWLOC_LIBS="-lhwloc"
                 LIBS="$LIBS $HWLOC_LIBS"]
		)]
	)

	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

	AC_CHECK_HEADERS([hwloc.h], [], [pwrapi_check_hwloc_happy="no"])
	AC_LINK_IFELSE([AC_LANG_PROGRAM([], [
			int a;
		])], [pwrapi_check_hwloc_lib_happy="yes"],
		[pwrapi_check_hwloc_lib_happy="no"])

	AS_IF([test "x$pwrapi_check_hwloc_lib_happy" = "xno"],
		[pwrapi_check_hwloc_happy="no"])

	AC_LANG_RESTORE

	CPPFLAGS="$CPPFLAGS_saved"
	LDFLAGS="$LDFLAGS_saved"
	LIBS="$LIBS_saved"

	AC_SUBST([HWLOC_CPPFLAGS])
	AC_SUBST([HWLOC_LDFLAGS])
	AC_SUBST([HWLOC_LIBS])

	AM_CONDITIONAL([HAVE_HWLOC], [test "x$pwrapi_check_hwloc_happy" = "xyes"])
	AS_IF([test "x$pwrapi_check_hwloc_happy" = "xyes"], 
		[AC_DEFINE([HAVE_HWLOC], [1], [Set to 1 if hwloc is found during configuration])])

	AS_IF([test "x$pwrapi_check_hwloc_happy" = "xyes"], [$1], [$2])
])
