bin_PROGRAMS = pwrapi pwrgrp pwrgen

# Power API Tools
pwrapi_SOURCES = pwrapi.c
pwrapi_CFLAGS = -I$(top_srcdir)/pwr
pwrapi_LDADD = $(top_srcdir)/pwr/libpwr.la
pwrgrp_SOURCES = pwrgrp.c
pwrgrp_CFLAGS = -I$(top_srcdir)/pwr
pwrgrp_LDADD = $(top_srcdir)/pwr/libpwr.la
pwrgen_SOURCES = pwrgen.cc
pwrgen_LDFLAGS = -ltinyxml2

