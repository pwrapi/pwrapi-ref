AUTOMAKE_OPTIONS = foreign subdir-objects
VERSION_INFO = 1:0:0

#DIST_SUBDIRS = pwr plugins tools examples
DIST_SUBDIRS = pwr plugins tools examples

SUBDIRS = pwr plugins tools

EXTRA_DIST = LICENSE Changes pwr.m4 autogen.sh  

include_HEADERS = pwr/pwr.h pwr/pwrtypes.h pwr/pwrdev.h

lib_LTLIBRARIES = libpwr.la libpwr_pgdev.la libpwr_pidev.la libpwr_rapldev.la libpwr_xtpmdev.la libpwr_wudev.la libpwr_cpudev.la libdummy_dev.la

# Power API Framework
libpwr_la_SOURCES = pwr/debug.cc pwr/pwr.cc pwr/cntxt.cc pwr/treeNode.cc pwr/deviceStat.cc pwr/xmlConfig.cc $(top_srcdir)/../tinyxml/tinyxml2.cpp
libpwr_la_CPPFLAGS = -I$(top_srcdir)/../tinyxml
libpwr_la_LDFLAGS = -version-info $(VERSION_INFO)

# Power API Plugins
libpwr_pgdev_la_SOURCES = plugins/pwr_dev.c plugins/pwr_pgdev.c
libpwr_pgdev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_pidev_la_SOURCES = plugins/pwr_dev.c plugins/pwr_pidev.c
libpwr_pidev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_rapldev_la_SOURCES = plugins/pwr_dev.c plugins/pwr_rapldev.c
libpwr_rapldev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_xtpmdev_la_SOURCES = plugins/pwr_dev.c plugins/pwr_xtpmdev.c
libpwr_xtpmdev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_wudev_la_SOURCES = plugins/pwr_dev.c plugins/pwr_wudev.c
libpwr_wudev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_cpudev_la_SOURCES = plugins/pwr_dev.c plugins/pwr_cpudev.c
libpwr_cpudev_la_CFLAGS = -I$(top_srcdir)/pwr
libdummy_dev_la_SOURCES = plugins/pwr_dev.c plugins/dummy_dev.c
libdummy_dev_la_CFLAGS = -I$(top_srcdir)/pwr

bin_PROGRAMS = pwrapi pwrgrp pwrgen snippet dummyTest

# Power API Tools
pwrapi_SOURCES = tools/pwrapi.c
pwrapi_CFLAGS = -I$(top_srcdir)/pwr
pwrapi_LDADD = libpwr.la
pwrgrp_SOURCES = tools/pwrgrp.c
pwrgrp_CFLAGS = -I$(top_srcdir)/pwr
pwrgrp_LDADD = libpwr.la
pwrgen_SOURCES = tools/pwrgen.cc 
pwrgen_CPPFLAGS = -I$(top_srcdir)/../tinyxml
pwrgen_LDADD = $(top_srcdir)/../tinyxml/libpwr_la-tinyxml2.lo

# Power API Examples
snippet_SOURCES = examples/snippet.c
snippet_CFLAGS = -I$(top_srcdir)/pwr
snippet_LDADD = libpwr.la
dummyTest_SOURCES = examples/dummyTest.c
dummyTest_CFLAGS = -I$(top_srcdir)/pwr
dummyTest_LDADD = libpwr.la


ACLOCAL_AMFLAGS = -I m4

