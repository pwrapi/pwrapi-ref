lib_LTLIBRARIES =	libpwr_rapldev.la \
			libpwr_apmdev.la \
			libpwr_xtpmdev.la \
			libpwr_pmcdev.la \
			libpwr_wudev.la \
			libpwr_cpudev.la \
			libpwr_powercapdev.la \
			libdummy_dev.la \
			libpwr_redfishdev.la

# Power API Plugins
if HAVE_POWERGADGET
lib_LTLIBRARIES += libpwr_pgdev.la

libpwr_pgdev_la_SOURCES = pwr_dev.c pwr_pgdev.c
libpwr_pgdev_la_CFLAGS = -I$(top_srcdir)/src/pwr $(POWERGADGET_CFLAGS)
libpwr_pgdev_la_LDFLAGS = -version-info 1:0:1 $(POWERGADGET_LDFLAGS) $(POWERGADGET_LIBS)
endif

if HAVE_PIAPI
lib_LTLIBRARIES += libpwr_piapidev.la

libpwr_piapidev_la_SOURCES = pwr_dev.c pwr_piapidev.c
libpwr_piapidev_la_CFLAGS = -I$(top_srcdir)/src/pwr $(PIAPI_CFLAGS)
libpwr_piapidev_la_LDFLAGS = -version-info 1:0:1 $(PIAPI_LDFLAGS) $(PIAPI_LIBS)
endif

if HAVE_POWERINSIGHT
lib_LTLIBRARIES += libpwr_pidev.la

libpwr_pidev_la_SOURCES = pwr_dev.c pwr_pidev.c
libpwr_pidev_la_CFLAGS = -I$(top_srcdir)/src/pwr $(POWERINSIGHT_CFLAGS)
libpwr_pidev_la_LDFLAGS = -version-info 1:0:1 $(POWERINSIGHT_LDFLAGS) $(POWERINSIGHT_LIBS)
endif

if HAVE_MIC
lib_LTLIBRARIES += libpwr_micdev.la

libpwr_micdev_la_SOURCES = pwr_dev.c pwr_micdev.c
libpwr_micdev_la_CFLAGS = -I$(top_srcdir)/src/pwr $(MIC_INCLUDE)
libpwr_micdev_la_LDFLAGS = -version-info 1:0:1 $(MIC_LDFLAGS) $(MIC_LIBS)
endif

libpwr_rapldev_la_SOURCES = pwr_dev.c pwr_rapldev.c
libpwr_rapldev_la_CFLAGS = -I$(top_srcdir)/src/pwr
libpwr_rapldev_la_LDFLAGS = -version-info 1:0:1

libpwr_apmdev_la_SOURCES = pwr_dev.c pwr_apmdev.c
libpwr_apmdev_la_CFLAGS = -I$(top_srcdir)/src/pwr
libpwr_apmdev_la_LDFLAGS = -version-info 1:0:1

libpwr_xtpmdev_la_SOURCES = pwr_dev.c pwr_xtpmdev.c
libpwr_xtpmdev_la_CFLAGS = -I$(top_srcdir)/src/pwr
libpwr_xtpmdev_la_LDFLAGS = -version-info 1:0:1

libpwr_pmcdev_la_SOURCES = pwr_dev.c pwr_pmcdev.c
libpwr_pmcdev_la_CFLAGS = -I$(top_srcdir)/src/pwr
libpwr_pmcdev_la_LDFLAGS = -version-info 1:0:1

libpwr_wudev_la_SOURCES = pwr_dev.c pwr_wudev.c
libpwr_wudev_la_CFLAGS = -I$(top_srcdir)/src/pwr
libpwr_wudev_la_LDFLAGS = -version-info 1:0:1

libpwr_cpudev_la_SOURCES = pwr_dev.c pwr_cpudev.c
libpwr_cpudev_la_CFLAGS = -I$(top_srcdir)/src/pwr
libpwr_cpudev_la_LDFLAGS = -version-info 1:0:1

libdummy_dev_la_SOURCES = pwr_dev.c dummy_dev.c
libdummy_dev_la_CFLAGS = -I$(top_srcdir)/src/pwr
libdummy_dev_la_LDFLAGS = -version-info 1:0:1

libpwr_powercapdev_la_SOURCES = pwr_dev.c pwr_powercap.c
libpwr_powercapdev_la_CFLAGS = -I$(top_srcdir)/src/pwr
libpwr_powercapdev_la_LDFLAGS = -version-info 1:0:1

libpwr_redfishdev_la_SOURCES = pwr_dev.c redfish_dev.c
libpwr_redfishdev_la_CFLAGS = -I$(top_srcdir)/src/pwr
libpwr_redfishdev_la_LDFLAGS = -version-info 1:0:1

