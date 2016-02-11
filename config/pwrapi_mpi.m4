
AC_DEFUN([PWRAPI_CHECK_MPI], [
  AC_ARG_WITH([mpi],
    [AS_HELP_STRING([--with-mpi@<:@=DIR@:>@],
      [Use MPI package installed in optionally specified DIR])])

  pwrapi_check_mpi_happy="yes"

  FOUND_MPI="no"
  MPI_EXE=""

  AC_PATH_PROG([MPI_EXE], ["mpicxx"], ["NOTFOUND"])

  AS_IF([test -n "$with_mpi"],
        [MPI_CPPFLAGS="-I$with_mpi/include"],
        [AS_IF([test "$MPI_EXE" != "NOTFOUND"],
                [MPI_CPPFLAGS=`$MPI_EXE --includes`])])

  AS_IF([test -n "$with_mpi"],
        [MPI_LDFLAGS="-L$with_mpi/lib"],
        [AS_IF([test "$MPI_EXE" != "NOTFOUND"],
                [MPI_LDFLAGS=`$MPI_EXE --ldflags`])])

  CPPFLAGS_saved="$CPPFLAGS"
  CPPFLAGS="$MPI_CPPFLAGS $CPPFLAGS"

  AC_LANG_PUSH(C++)
  AC_CHECK_HEADERS([mpi.h], [pwrapi_check_mpi_happy="yes"], [pwrapi_check_mpi_happy="no"])
  AC_LANG_POP(C++)

  CPPFLAGS="$CPPFLAGS_saved"

  AC_SUBST([MPI_CPPFLAGS])
  AC_SUBST([MPI_LDFLAGS])
  AC_SUBST([MPI_CONFIG_EXE])

  AM_CONDITIONAL([HAVE_MPI], [test "$pwrapi_check_mpi_happy" = "yes"])

  AS_IF([test "$pwrapi_check_mpi_happy" = "yes"],
        [AC_DEFINE([HAVE_MPI], [1], [Set to 1 if MPI was found])])
  AS_IF([test "$pwrapi_check_mpi_happy" = "yes"], [$1], [$2])
])
