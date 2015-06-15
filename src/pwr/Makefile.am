lib_LTLIBRARIES = libpwr.la

include_HEADERS = pwr.h pwrtypes.h pwrdev.h

# Power API Framework
libpwr_la_SOURCES = debug.cc pwr.cc cntxt.cc treeNode.cc deviceStat.cc xmlConfig.cc

libpwr_la_LDFLAGS = $(LDFLAGS) -version-info 1:0:1
libpwr_la_CPPFLAGS = $(CPPFLAGS) -I$(top_srcdir)/src/tinyxml2
libpwr_la_LIBADD = $(top_builddir)/src/tinyxml2/libtinyxml2.la

if HAVE_XMLRPC
libpwr_la_SOURCES += rpcTreeNode.cc 
libpwr_la_CPPFLAGS += $(XMLRPC_CPPFLAGS)
libpwr_la_LDFLAGS += $(XMLRPC_LDFLAGS)
libpwr_la_LIBADD += $(XMLRPC_LIBS)
endif

if USE_STATIC
libpwr_la_SOURCES += static.cc
else
libpwr_la_SOURCES += dynamic.cc
libpwr_la_LIBADD += -ldl
endif
