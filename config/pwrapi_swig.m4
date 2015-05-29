
AC_DEFUN([PWRAPI_CHECK_SWIG], [
	AC_ARG_WITH([swig], [AS_HELP_STRING([--with-swig], [Use the swig binary by path])])

	AC_MSG_NOTICE([Checking for swig to enable Python bindings])
	pwrapi_check_swig_happy="yes"
	PWRAPI_CHECK_PYTHON([pwrapi_check_swig_happy="yes"], [pwrapi_check_swig_happy="no"], [])

	SWIG=""
	SWIG_FLAGS="-python"
	SWIG_PYTHON_CPPFLAGS="$PYTHON_CPPFLAGS"

	AS_IF( 	
	     [ test "x$with_swig" = "xyes" ], [ AC_PATH_PROG([SWIG], [swig], [""]) ],
	     [ AS_IF( [ test "x$with_swig" = "x" ], [ AC_PATH_PROG([SWIG], [swig], [""]) ],
		[SWIG="$with_swig"] )]
	     )

	AS_IF( [test "x$SWIG" = "x"], [pwrapi_check_swig_happy="no"] )

	AC_SUBST([SWIG])
	AC_SUBST([SWIG_FLAGS])
	AC_SUBST([SWIG_PYTHON_CPPFLAGS])

	AM_CONDITIONAL([HAVE_SWIG], [test "x$pwrapi_check_swig_happy" = "xyes"])

	AS_IF([test "x$pwrapi_check_swig_happy" = "xyes"],
		[ AC_DEFINE([HAVE_SWIG], [1], [Set to 1 if SWIG is found during configure]) ] )
	AS_IF([test "x$pwrapi_check_swig_happy" = "xyes"], [$1], [$2])

	AC_MSG_NOTICE([Completed check for swig.])
])
