

AC_DEFUN([PWRAPI_CHECK_PYTHON], [
  AC_ARG_WITH([python],
    [AS_HELP_STRING([--with-python@<:@=DIR@:>@],
      [Use Python package installed in optionally specified DIR])])

  AC_REQUIRE([AM_PATH_PYTHON])

  pwrapi_check_python_happy="yes"

  FOUND_PYTHON="no"
  PYTHON_CONFIG_EXE=""

  AC_PATH_PROG([PYTHON_CONFIG_EXE], ["python-config"], ["NOTFOUND"])

  AS_IF([test -n "$with_python"],
        [PYTHON_CPPFLAGS="-I$with_python/include"],
        [AS_IF([test "$PYTHON_CONFIG_EXE" != "NOTFOUND"],
                [PYTHON_CPPFLAGS=`$PYTHON_CONFIG_EXE --includes`])])

  AS_IF([test -n "$with_python"],
        [PYTHON_LDFLAGS="-L$with_python/lib"],
        [AS_IF([test "$PYTHON_CONFIG_EXE" != "NOTFOUND"],
                [PYTHON_LDFLAGS=`$PYTHON_CONFIG_EXE --ldflags`])])

  CPPFLAGS_saved="$CPPFLAGS"
  CPPFLAGS="$PYTHON_CPPFLAGS $CPPFLAGS"

  AC_LANG_PUSH(C++)
  AC_CHECK_HEADERS([Python.h], [pwrapi_check_python_happy="yes"], [pwrapi_check_python_happy="no"])
  AC_LANG_POP(C++)

  CPPFLAGS="$CPPFLAGS_saved"

  AC_SUBST([PYTHON_CPPFLAGS])
  AC_SUBST([PYTHON_LDFLAGS])
  AC_SUBST([PYTHON_CONFIG_EXE])

  AM_CONDITIONAL([HAVE_PYTHON], [test "$pwrapi_check_python_happy" = "yes"])

  AS_IF([test "$pwrapi_check_python_happy" = "yes"],
        [AC_DEFINE([HAVE_PYTHON], [1], [Set to 1 if Python was found])])
  AS_IF([test "$pwrapi_check_python_happy" = "yes"], [$1], [$2])
])
