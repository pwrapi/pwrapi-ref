
bin_PROGRAMS = pwrdaemon

# Power API Tools
pwrdaemon_SOURCES = \
	pwrdaemon.cc \
	router/router.cc \
	router/torusCore.cc \
	router/routerSelect.cc \
	router/allocEvent.cc \
	router/client.cc \
	router/commCreateEvent.cc \
	server/server.cc \
	server/allocEvent.cc

pwrdaemon_CPPFLAGS = $(CPPFLAGS) -I$(top_srcdir)/src/pwr \
				   -I$(top_srcdir)/tools/pwrdaemon/router \
				   -I$(top_srcdir)/tools/pwrdaemon/server \
				   -I$(top_srcdir)/src/tinyxml2

pwrdaemon_LDADD = $(top_builddir)/src/pwr/libpwr.la \
				  $(top_builddir)/src/tinyxml2/libtinyxml2.la
pwrdaemon_LDFLAGS = -lpthread
