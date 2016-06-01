orig_CFLAGS="$CFLAGS"

AC_INIT([pwrapi], [1.0.1])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config])
AM_INIT_AUTOMAKE([subdir-objects])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([src/pwr_config.h])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
dnl Supply default CFLAGS, if not specified
if test -z "$orig_CFLAGS"; then
  if test x$GCC = xyes; then
    CFLAGS="-g -O3"
  fi
fi

AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MKDIR_P
AC_PROG_LN_S

AC_C_RESTRICT
AC_C_INLINE

m4_include([config/pwrapi.m4])
m4_include([config/pwrapi_python.m4])
m4_include([config/pwrapi_swig.m4])
m4_include([config/pwrapi_ulxmlrpcpp.m4])
m4_include([config/pwrapi_piapi.m4])
m4_include([config/pwrapi_powerinsight.m4])
m4_include([config/pwrapi_powergadget.m4])
m4_include([config/acx_mpi.m4])
m4_include([config/pwrapi_mpi.m4])
m4_include([config/pwrapi_hwloc.m4])
m4_include([config/pwrapi_mic.m4])

PWRAPI_CHECK_HWLOC([pwrapi_have_hwloc="yes"], [pwrapi_have_hwloc="no"])
PWRAPI_CHECK_SWIG([pwrapi_have_swig="yes"], [pwrapi_have_swig="no"])
PWRAPI_CHECK_ULXMLRPCPP([pwrapi_have_xmlrpc="yes"], [pwrapi_have_xmlrpc="no"])

PWRAPI_CHECK_PIAPI([pwrapi_have_piapi="yes"], [pwrapi_have_piapi="no"])
PWRAPI_CHECK_POWERINSIGHT([pwrapi_have_powerinsight="yes"], [pwrapi_have_powerinsight="no"])
PWRAPI_CHECK_POWERGADGET([pwrapi_have_powergadget="yes"], [pwrapi_have_powergadget="no"])

PWRAPI_CHECK_MPI([pwrapi_have_mpi="yes"], [pwrapi_have_mpi="no"])

PWRAPI_CHECK_MIC([pwrapi_have_mic="yes"], [pwrapi_have_mic="no"])

PWRAPI_CHECK_DEBUG()

AC_CONFIG_FILES([Makefile
                 tools/script/hwloc_profile
                 tools/script/hwloc_powercap_profile
                 tools/script/hwloc_cpu_profile
                 tools/script/hwloc_rapl_profile
                 tools/script/hwloc_multi_profile
                 examples/script/xml_profile
                 src/Makefile
                 src/tinyxml2/Makefile
                 src/pwr/Makefile
                 src/bindings/python/Makefile
                 src/plugins/Makefile
                 tools/Makefile
                 tools/pwrdaemon/Makefile
                 examples/Makefile
                 test/Makefile])

AC_OUTPUT
