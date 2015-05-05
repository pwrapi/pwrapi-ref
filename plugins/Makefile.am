#
# Copyright 2014 Sandia Corporation. Under the terms of Contract
# DE-AC04-94AL85000, there is a non-exclusive license for use of this work
# by or on behalf of the U.S. Government. Export of this program may require
# a license from the United States Government.
#
# This file is part of the Power API Prototype software package. For license
# information, see the LICENSE file in the top level directory of the
# distribution.
#

#AM_CXXFLAGS = "" 
AM_CPPFLAGS = -I$(top_srcdir)
AM_LDFLAGS = $(LIBTOOL_WRAPPER_LDFLAGS)
LDADD = $(top_builddir)/libpwr.la


MACOS=
PI=y
DEBUG=

CFLAGS=-Wall -I../pwr -fPIC

if BUILD_DEBUG
CFLAGS+=-DVERBOSE
endif

CXXFLAGS=${CFLAGS}
LDFLAGS=${CFLAGS} -L. -lm -pthread

all: libdummy_dev.so libpwr_pidev.so libpwr_rapldev.so libpwr_xtpmdev.so libpwr_wudev.so libpwr_pgdev.so libpwr_cpudev.so

libdummy_dev.so: pwr_dev.o dummy_dev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o dummy_dev.o

if BUILD_MACOS
libpwr_pgdev.so: pwr_dev.o pwr_pgdev.o
	$(CC) -shared $(CFLAGS) -framework IntelPowerGadget -o $@ pwr_dev.o pwr_pgdev.o
else
libpwr_pgdev.so:
endif

if BUILD_PIDEV
libpwr_pidev.so: pwr_dev.o pwr_pidev.o
	$(CC) -shared $(CFLAGS) -lpiapi -o $@ pwr_dev.o pwr_pidev.o
else
libpwr_pidev.so:
endif

libpwr_rapldev.so: pwr_dev.o pwr_rapldev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o pwr_rapldev.o

libpwr_xtpmdev.so: pwr_dev.o pwr_xtpmdev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o pwr_xtpmdev.o

libpwr_wudev.so: pwr_dev.o pwr_wudev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o pwr_wudev.o

libpwr_cpudev.so: pwr_dev.o pwr_cpudev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o pwr_cpudev.o

clean:
	rm -f *_*dev.o lib*_*dev.so

