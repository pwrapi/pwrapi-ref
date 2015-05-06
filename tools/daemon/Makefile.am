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

DEBUG =

ROOT=${PWD}/..

ULXMLRCP= ${HOME}/ulxmlrpcpp-1.7.5/install

LDLIBS = -lpwr -ldl -lulxmlrpcpp
CPPFLAGS = -I ${ULXMLRCP}/include  -I../pwr
CFLAGS = -Wall 
LDFLAGS = -L${ROOT}/pwr  -L ${ULXMLRCP}/lib
CXXFLAGS = -Wall -I

ifeq ($(DEBUG),y)
CPPFLAGS += -DDEBUG_ON
endif

all: test

test: server.cpp
	$(CXX) -Wall $(CPPFLAGS) $(LDFLAGS) server.cpp $(LDLIBS)  -o server 

clean:
	rm -f server 
