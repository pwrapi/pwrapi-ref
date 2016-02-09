if HAVE_PYTHON
if HAVE_MPI
lib_LTLIBRARIES = libpwrrt.la
endif
endif

bin_PROGRAMS = pwrdaemon

# Power API Tools
pwrdaemon_SOURCES = \
	pwrdaemon.cc \
	router/router.cc \
	router/torusCore.cc \
	router/treeCore.cc \
	router/routerSelect.cc \
	router/allocEvent.cc \
	router/client.cc \
	router/commCreateEvent.cc \
	server/server.cc \
	server/allocEvent.cc \
	logger/logger.cc

pwrdaemon_CPPFLAGS = $(CPPFLAGS) -I$(top_srcdir)/src/pwr \
				   -I$(top_srcdir)/tools/pwrdaemon/router \
				   -I$(top_srcdir)/tools/pwrdaemon/server \
				   -I$(top_srcdir)/tools/pwrdaemon/logger \
				   -I$(top_srcdir)/src/tinyxml2 -Wall -fno-strict-aliasing    

pwrdaemon_LDADD = $(top_builddir)/src/pwr/libpwr.la \
				  $(top_builddir)/src/tinyxml2/libtinyxml2.la

pwrdaemon_LDFLAGS = -lpthread

if HAVE_PYTHON
if HAVE_MPI
libpwrrt_la_SOURCES = powerrt.cc
libpwrrt_la_LDFLAGS = $(PYTHON_LDFLAGS) 
libpwrrt_la_CPPFLAGS = $(PYTHON_CPPFLAGS) $(MPI_CPPFLAGS)
endif
endif

if HAVE_PYTHON
pwrdaemon_CPPFLAGS += $(PYTHON_CPPFLAGS)
pwrdaemon_LDFLAGS += $(PYTHON_LDFLAGS)
endif
