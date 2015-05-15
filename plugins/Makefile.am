lib_LTLIBRARIES =	libpwr_pgdev.la \
			libpwr_pidev.la \
			libpwr_rapldev.la \
			libpwr_xtpmdev.la \
			libpwr_wudev.la \
			libpwr_cpudev.la \
			libdummy_dev.la

# Power API Plugins
if BUILD_MACOS
libpwr_pgdev_la_SOURCES = pwr_dev.c pwr_pgdev.c
libpwr_pgdev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_pgdev_la_LDFLAGS = -version-info 1:0:0
endif

libpwr_pidev_la_SOURCES = pwr_dev.c pwr_pidev.c
libpwr_pidev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_pidev_la_LDFLAGS = -version-info 1:0:0

libpwr_rapldev_la_SOURCES = pwr_dev.c pwr_rapldev.c
libpwr_rapldev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_rapldev_la_LDFLAGS = -version-info 1:0:0

libpwr_xtpmdev_la_SOURCES = pwr_dev.c pwr_xtpmdev.c
libpwr_xtpmdev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_xtpmdev_la_LDFLAGS = -version-info 1:0:0

libpwr_wudev_la_SOURCES = pwr_dev.c pwr_wudev.c
libpwr_wudev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_wudev_la_LDFLAGS = -version-info 1:0:0

libpwr_cpudev_la_SOURCES = pwr_dev.c pwr_cpudev.c
libpwr_cpudev_la_CFLAGS = -I$(top_srcdir)/pwr
libpwr_cpudev_la_LDFLAGS = -version-info 1:0:0

libdummy_dev_la_SOURCES = pwr_dev.c dummy_dev.c
libdummy_dev_la_CFLAGS = -I$(top_srcdir)/pwr
libdummy_dev_la_LDFLAGS = -version-info 1:0:0


