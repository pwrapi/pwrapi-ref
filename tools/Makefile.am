bin_PROGRAMS = pwrapi pwrgrp

# Power API Tools
pwrapi_SOURCES = pwrapi.c
pwrapi_CFLAGS = -I$(top_srcdir)/src/pwr
pwrapi_LDADD = $(top_builddir)/src/pwr/libpwr.la

pwrgrp_SOURCES = pwrgrp.c
pwrgrp_CFLAGS = -I$(top_srcdir)/src/pwr
pwrgrp_LDADD = $(top_builddir)/src/pwr/libpwr.la

man_MANS = pwrapi.8 pwrgrp.8

if USE_XML 
bin_PROGRAMS += pwrgen

pwrgen_SOURCES = pwrgen.cc
pwrgen_LDFLAGS = -ltinyxml2

man_MANS += pwrgen.8
endif

if USE_XMLRPC
bin_PROGRAMS += pwrsrv

pwrsrv_SOURCES = pwrsrv.cpp
pwrsrv_CPPFLAGS = -I$(top_srcdir)/src/pwr
pwrsrv_LDADD = $(top_builddir)/src/pwr/libpwr.la
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
