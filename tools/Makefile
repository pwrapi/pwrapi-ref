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

XMLRPC =
SHARED ?= y

CXXFLAGS = -Wall -I../pow
LDFLAGS = -L../pow -lpow -lm -pthread

ifeq ($(SHARED),y)
CC=g++
CXXFLAGS += -fPIC
LDFLAGS += -ldl
else
CC=g++ -static
LDFLAGS += -L../plugins -lpwr_pidev -L$(HOME) -lpiapi
endif

ifeq ($(XMLRPC),y)
LIB_SRC += rpcTreeNode.cc
ULXMLRPC=$(HOME)/ulxmlrpcpp-1.7.5/install
CPPFLAGS += -DUSE_ULXMLRPC -I$(ULXMLRPC)/include
CPPFLAGS += -DUSE_RPC
LDFLAGS += -L$(ULXMLRPC)/lib
LDFLAGS += -lulxmlrpcpp
endif


EXES=powerapi powergrp
all:  ${EXES}

powergrp: powergrp.c
	$(CC) ${CXXFLAGS} powergrp.c -o $@  ${LDFLAGS}

powerapi: powerapi.c
	$(CC) ${CXXFLAGS} powerapi.c -o $@  ${LDFLAGS}

clean:
	rm -f  ${EXES}
