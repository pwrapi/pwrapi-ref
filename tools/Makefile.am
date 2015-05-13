bin_PROGRAMS = pwrapi pwrgrp pwrsrv pwrgen pypwr

# Power API Tools
pwrapi_SOURCES = pwrapi.c
pwrapi_CFLAGS = -I$(top_srcdir)/pwr
pwrapi_LDADD = $(top_srcdir)/pwr/libpwr.la

pwrgrp_SOURCES = pwrgrp.c
pwrgrp_CFLAGS = -I$(top_srcdir)/pwr
pwrgrp_LDADD = $(top_srcdir)/pwr/libpwr.la

pwrsrv_SOURCES = pwrsrv.c
pwrsrv_CFLAGS = -I$(top_srcdir)/pwr
pwrsrv_LDADD = $(top_srcdir)/pwr/libpwr.la
pwrsrv_LDFLAGS = -lulxmlrpcpp

pwrgen_SOURCES = pwrgen.cc
pwrgen_LDFLAGS = -ltinyxml2

pypwr_SOURCES = pypwr.c
pypwr_CFLAGS = -I$(top_srcdir)/pwr
pypwr_LDADD = $(top_srcdir)/pwr/libpwr.la
pypwr_LDFLAGS = -l

