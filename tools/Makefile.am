SUBDIRS = pwrdaemon
DIST_SUBDRIS = pwrdaemon

bin_PROGRAMS = pwrapi pwrgen pwrdmp pwrls foo 

scriptdir = $(prefix)/script
script_DATA = script/hwloc_profile\
         script/hwloc_powercap_profile

# Power API Tools
pwrapi_SOURCES = pwrapi.c
pwrapi_CFLAGS = -I$(top_srcdir)/src/pwr
pwrapi_LDADD = $(top_builddir)/src/pwr/libpwr.la

pwrgen_SOURCES = pwrgen.cc
pwrgen_CPPFLAGS = -I$(top_srcdir)/src/tinyxml2
pwrgen_LDADD = $(top_builddir)/src/tinyxml2/libtinyxml2.la

pwrdmp_SOURCES = pwrdmp.c
pwrdmp_CFLAGS = -I$(top_srcdir)/src/pwr
pwrdmp_LDADD = $(top_builddir)/src/pwr/libpwr.la

pwrls_SOURCES = pwr-ls.c
pwrls_CFLAGS = -I$(top_srcdir)/src/pwr
pwrls_LDADD = $(top_builddir)/src/pwr/libpwr.la

foo_SOURCES = test.c
foo_CFLAGS = -I$(top_srcdir)/src/pwr
foo_LDADD = $(top_builddir)/src/pwr/libpwr.la

man_MANS = pwrapi.8 pwrgen.8 pwrdmp.8

if HAVE_XMLRPC
bin_PROGRAMS += pwrsrv

pwrsrv_SOURCES = pwrsrv.cpp
pwrsrv_CPPFLAGS = $(XMLRPC_CPPFLAGS) -I$(top_srcdir)/src/pwr
pwrsrv_LDFLAGS = $(XMLRPC_LDFLAGS)
pwrsrv_LDADD = $(XMLRPC_LIBS) $(top_builddir)/src/pwr/libpwr.la

man_MANS += pwrsrv.8
endif

configdir = $(prefix)/config
config_DATA = 	config/shepard-platform.xml \
		config/shepard-node.xml \
		config/cooper-platform.xml \
		config/cooper-node.xml \
		config/teller-platform.xml \
		config/teller-node.xml \
		config/powercap.hwloc \
		config/dummyNode.hwloc

EXTRA_DIST = $(config_DATA) $(script_DATA)
