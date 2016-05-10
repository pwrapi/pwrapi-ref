
AC_DEFUN([PWRAPI_CHECK_MIC], [
	AC_ARG_WITH([mic], [AS_HELP_STRING([--with-mic@<:@=DIR@:>@], [Use the mic binding in directory specified (DIR).])])

	pwrapi_check_mic_happy="yes"

	CPPFLAGS_saved="$CPPFLAGS"
  	LDFLAGS_saved="$LDFLAGS"
	LIBS_saved="$LIBS"
	MIC_LIBS=""

	AS_IF([test -z "$with_mic"],
		[MIC_CPPFLAGS=
                 MIC_LDFLAGS=
                 MIC_LIBS="-lscif -lmicmgmt"
                 LIBS="$LIBS $MIC_LIBS"],
		[	AS_IF([test "x$with_mic" = "xyes"],
				[MIC_CPPFLAGS=
           		 MIC_LDFLAGS=
           		 MIC_LIBS="-lscif -lmicmgmt"
           		 LIBS="$LIBS $MIC_LIBS"],
				[MIC_CPPFLAGS="-I$with_mic/include"
                 CPPFLAGS="$MIC_CPPFLAGS $CPPFLAGS"
                 MIC_LDFLAGS="-L$with_mic/lib"
                 LDFLAGS="$MIC_LDFLAGS $LDFLAGS"
                 MIC_LIBS="-lscif -lmicmgmt"
                 LIBS="$LIBS $MIC_LIBS"]
		)]
	)

	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

	AC_CHECK_HEADERS([miclib.h], [], [pwrapi_check_mic_happy="no"])
	AC_LINK_IFELSE([AC_LANG_PROGRAM([], [
			int a;
		])], [pwrapi_check_mic_lib_happy="yes"],
		[pwrapi_check_mic_lib_happy="no"])

	AS_IF([test "x$pwrapi_check_mic_lib_happy" = "xno"],
		[pwrapi_check_mic_happy="no"])

	AC_LANG_RESTORE

	CPPFLAGS="$CPPFLAGS_saved"
	LDFLAGS="$LDFLAGS_saved"
	LIBS="$LIBS_saved"

	AC_SUBST([MIC_CPPFLAGS])
	AC_SUBST([MIC_LDFLAGS])
	AC_SUBST([MIC_LIBS])

	AM_CONDITIONAL([HAVE_MIC], [test "x$pwrapi_check_mic_happy" = "xyes"])
	AS_IF([test "x$pwrapi_check_mic_happy" = "xyes"], 
		[AC_DEFINE([HAVE_MIC], [1], [Set to 1 if mic is found during configuration])])

	AS_IF([test "x$pwrapi_check_mic_happy" = "xyes"], [$1], [$2])
])
