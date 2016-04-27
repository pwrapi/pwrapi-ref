lib_LTLIBRARIES = libpwr.la

include_HEADERS = pwr.h pwrtypes.h pwrdev.h eventChannel.h events.h event.h eventType.h serialize.h tcpEventChannel.h util.h xmlConfig.h config.h debug.h

# Power API Framework
libpwr_la_SOURCES = debug.cc pwr.cc cntxt.cc object.cc xmlConfig.cc deviceStat.cc
libpwr_la_SOURCES += distCntxt.cc distComm.cc distRequest.cc distObject.cc eventChannel.cc tcpEventChannel.cc allocEvent.cc distGroup.cc distGrpComm.cc

libpwr_la_LDFLAGS = $(LDFLAGS) -version-info 1:0:1
libpwr_la_CPPFLAGS = $(CPPFLAGS) -I$(top_srcdir)/src/tinyxml2 -Wall -fno-strict-aliasing
libpwr_la_LIBADD = $(top_builddir)/src/tinyxml2/libtinyxml2.la

if HAVE_HWLOC
include_HEADERS += hwlocConfig.h
libpwr_la_SOURCES += hwlocConfig.cc
libpwr_la_CPPFLAGS += $(HWLOC_CPPFLAGS)
libpwr_la_LDFLAGS += $(HWLOC_LDFLAGS)
libpwr_la_LIBADD += $(HWLOC_LIBS)
endif

if HAVE_PYTHON
include_HEADERS += pyConfig.h
libpwr_la_SOURCES += pyConfig.cc
libpwr_la_LDFLAGS += $(PYTHON_LDFLAGS) 
libpwr_la_CPPFLAGS += $(PYTHON_CPPFLAGS)
endif

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
