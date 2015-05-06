lib_LTLIBRARIES = libpwr.la

include_HEADERS = pwr.h pwrtypes.h pwrdev.h

# Power API Framework
libpwr_la_SOURCES = debug.cc pwr.cc cntxt.cc treeNode.cc deviceStat.cc xmlConfig.cc $(top_srcdir)/../tinyxml/tinyxml2.cpp
libpwr_la_CPPFLAGS = -I$(top_srcdir)/../tinyxml
libpwr_la_LDFLAGS = -version-info $(VERSION_INFO)

