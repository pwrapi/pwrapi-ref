lib_LTLIBRARIES = libpwr.la

include_HEADERS = pwr.h pwrtypes.h pwrdev.h

# Power API Framework
libpwr_la_SOURCES = debug.cc pwr.cc cntxt.cc treeNode.cc deviceStat.cc xmlConfig.cc $(top_srcdir)/../tinyxml/tinyxml2.cpp
if BUILD_SHARED
libpwr_la_SOURCES += dynamic.cc
else
libpwr_la_SOURCES += static.cc
endif
libpwr_la_CPPFLAGS = -I$(top_srcdir)/../tinyxml

