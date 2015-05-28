lib_LTLIBRARIES = libpwr.la

include_HEADERS = pwr.h pwrtypes.h pwrdev.h

# Power API Framework
libpwr_la_SOURCES = debug.cc pwr.cc cntxt.cc treeNode.cc deviceStat.cc nullConfig.cc

libpwr_la_LDFLAGS = -version-info 1:0:0
libpwr_la_LIBADD =
if BUILD_XML
libpwr_la_SOURCES += xmlConfig.cc
libpwr_la_LIBADD += -ltinyxml2 
endif

if BUILD_STATIC
libpwr_la_SOURCES += static.cc
else
libpwr_la_SOURCES += dynamic.cc
libpwr_la_LIBADD += -ldl
endif
