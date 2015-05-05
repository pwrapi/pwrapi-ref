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

XMLRPC =
SHARED ?= y

CXXFLAGS = -Wall -I../pwr
LDFLAGS = -L../pwr -lpwr -lm -pthread
LIB_SRC =

if BUILD_SHARED
CXXFLAGS += -fPIC
LDFLAGS += -ldl
else
CXXFLAGS += -static
LDFLAGS += -L../plugins -lpwr_pidev -L$(HOME) -lpiapi
endif

if BUILD_XMLRPC
LIB_SRC += rpcTreeNode.cc
ULXMLRPC=$(HOME)/ulxmlrpcpp-1.7.5/install
CPPFLAGS += -DUSE_ULXMLRPC -I$(ULXMLRPC)/include
CPPFLAGS += -DUSE_RPC
LDFLAGS += -L$(ULXMLRPC)/lib
LDFLAGS += -lulxmlrpcpp
endif


EXES=pwrapi pwrgrp
all:  ${EXES}

pwrgrp: pwrgrp.c
	$(CC) ${CXXFLAGS} pwrgrp.c -o $@  ${LDFLAGS}

pwrapi: pwrapi.c
	$(CC) ${CXXFLAGS} pwrapi.c -o $@  ${LDFLAGS}

clean:
	rm -f  ${EXES}
