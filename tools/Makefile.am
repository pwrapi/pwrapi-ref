bin_PROGRAMS = pwrapi pwrgrp pwrgen

# Power API Tools
pwrapi_SOURCES = pwrapi.c
pwrapi_CFLAGS = -I$(top_srcdir)/pwr
pwrapi_LDADD = $(top_builddir)/pwr/libpwr.la

pwrgrp_SOURCES = pwrgrp.c
pwrgrp_CFLAGS = -I$(top_srcdir)/pwr
pwrgrp_LDADD = $(top_builddir)/pwr/libpwr.la

pwrgen_SOURCES = pwrgen.cc
pwrgen_LDFLAGS = -ltinyxml2

man_MANS = pwrapi.8 pwrgrp.8 pwrgen.8

if BUILD_DAEMON
bin_PROGRAMS += pwrsrv

pwrsrv_SOURCES = pwrsrv.cpp
pwrsrv_CPPFLAGS = -I$(top_srcdir)/pwr
pwrsrv_LDADD = $(top_builddir)/pwr/libpwr.la
pwrsrv_LDFLAGS = -lulxmlrpcpp

man_MANS += pwrsrv.8
endif

configdir = $(docdir)/config
config_DATA = 	config/shepard-platform.xml \
		config/shepard-node.xml \
		config/cooper-platform.xml \
		config/cooper-node.xml \
		config/teller-platform.xml \
		config/teller-node.xml

EXTRA_DIST = $(config_DATA)

if BUILD_SWIG
bin_PROGRAMS += pypwr

EXTRA_DIST += pypwr.i
python_PYTHON = pypwr.py

pypwr_SOURCES = pypwr.cxx
pypwr_CFLAGS = -I$(top_srcdir)/pwr
pypwr_LDADD = $(top_builddir)/pwr/libpwr.la
endif

