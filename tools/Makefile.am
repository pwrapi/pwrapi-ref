bin_PROGRAMS = pwrapi pwrgrp pwrgen pwrsrv # pypwr

# Power API Tools
pwrapi_SOURCES = pwrapi.c
pwrapi_CFLAGS = -I$(top_srcdir)/pwr
pwrapi_LDADD = $(top_builddir)/pwr/libpwr.la

pwrgrp_SOURCES = pwrgrp.c
pwrgrp_CFLAGS = -I$(top_srcdir)/pwr
pwrgrp_LDADD = $(top_builddir)/pwr/libpwr.la

pwrsrv_SOURCES = pwrsrv.cpp
pwrsrv_CPPFLAGS = -I$(top_srcdir)/pwr
pwrsrv_LDADD = $(top_builddir)/pwr/libpwr.la
pwrsrv_LDFLAGS = -lulxmlrpcpp

pwrgen_SOURCES = pwrgen.cc
pwrgen_LDFLAGS = -ltinyxml2

#EXTRA_DIST = pypwr.i
#python_PYTHON = pypwr.py

#pypwr_SOURCES = pypwr.cxx
#pypwr_CFLAGS = -I$(top_srcdir)/pwr
#pypwr_LDADD = $(top_builddir)/pwr/libpwr.la

