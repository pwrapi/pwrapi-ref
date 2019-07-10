
AC_DEFUN([PWRAPI_CHECK_TX2MON], [
	AC_ARG_WITH([tx2mon], [AS_HELP_STRING([--with-tx2mon@<:@=DIR@:>@], [Use the tx2mon binding in directory specified (DIR).])])

	pwrapi_check_tx2mon_happy="yes"

	CPPFLAGS_saved="$CPPFLAGS"
  	LDFLAGS_saved="$LDFLAGS"
	LIBS_saved="$LIBS"
	TX2MON_LIBS=""

	AS_IF([test -z "$with_tx2mon"],
		[TX2MON_CPPFLAGS= TX2MON_LDFLAGS= TX2MON_LIBS= ],
		[	AS_IF([test "x$with_tx2mon" = "xyes"],
				[TX2MON_CPPFLAGS= TX2MON_LDFLAGS=  TX2MON_LIBS=],
				[TX2MON_CPPFLAGS="-I$with_tx2mon" CPPFLAGS="$TX2MON_CPPFLAGS $CPPFLAGS" TX2MON_LDFLAGS=  TX2MON_LIBS= ]
		)]
	)

	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS

	AC_CHECK_HEADERS([mc_oper_region.h], [], [pwrapi_check_tx2mon_happy="no"])
	AC_LINK_IFELSE([AC_LANG_PROGRAM([], [
                        int a;
                ])], [pwrapi_check_tx2mon_lib_happy="yes"],
                [pwrapi_check_tx2mon_lib_happy="no"])

	AS_IF([test "x$pwrapi_check_tx2mon_lib_happy" = "xno"],
		[pwrapi_check_tx2mon_happy="no"])

	AC_LANG_RESTORE

	CPPFLAGS="$CPPFLAGS_saved"
	LDFLAGS="$LDFLAGS_saved"
	LIBS="$LIBS_saved"

	AC_SUBST([TX2MON_CPPFLAGS])
	AC_SUBST([TX2MON_LDFLAGS])
	AC_SUBST([TX2MON_LIBS])

	AM_CONDITIONAL([HAVE_TX2MON], [test "x$pwrapi_check_tx2mon_happy" = "xyes"])
	AS_IF([test "x$pwrapi_check_tx2mon_happy" = "xyes"], 
		[AC_DEFINE([HAVE_TX2MON], [1], [Set to 1 if tx2mon is found during configuration])])

	AS_IF([test "x$pwrapi_check_tx2mon_happy" = "xyes"], [$1], [$2])
])
