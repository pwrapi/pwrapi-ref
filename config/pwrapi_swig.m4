
AC_DEFUN([PWRAPI_CHECK_SWIG], [
	AC_ARG_WITH([swig], [AS_HELP_STRING([--with-swig], [Use the swig binary by path])],[

	SWIG=""
	SWIG_FLAGS="-python"
	SWIG_PYTHON_CPPFLAGS="$PYTHON_CPPFLAGS"

	AS_IF( 	
	  [ test "x$with_swig" = "xyes" ], [ AC_PATH_PROG([SWIG], [swig], [""]) ],
	  [ AS_IF( [ test "x$with_swig" = "x" ], [ AC_PATH_PROG([SWIG], [swig], [""]) ],
      [SWIG="$with_swig"] )]
	)

	AS_IF( [test "x$SWIG" = "x"], [pwrapi_check_swig_happy="no"], [pwrapi_check_swig_happy="yes"] )

	AC_SUBST([SWIG])
	AC_SUBST([SWIG_FLAGS])
	AC_SUBST([SWIG_PYTHON_CPPFLAGS])

	AM_CONDITIONAL([HAVE_SWIG], [test "x$pwrapi_check_swig_happy" = "xyes"])

	AS_IF(
		[test "x$pwrapi_check_swig_happy" = "xyes"],
		[
			[$1]
			AC_DEFINE( [HAVE_SWIG], [1], [Set to 1 if SWIG is found during configure] ) 
		],
		[$2]
	)
],
[
  AM_CONDITIONAL(HAVE_SWIG, false)
  [$2]
]

)

])
