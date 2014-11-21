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

CC=gcc
CFLAGS = -Wall -I../pow -fPIC
CXXFLAGS = ${CFLAGS}
LDFLAGS = ${CFLAGS} -L../pow -lpow -lm -ldl -pthread

ifeq ($(XMLRPC),y)
LIB_SRC += rpcTreeNode.cc
ULXMLRPC=$(HOME)/ulxmlrpcpp-1.7.5/install
CPPFLAGS += -DUSE_ULXMLRPC -I$(ULXMLRPC)/include
CPPFLAGS += -DUSE_RPC
LDFLAGS += -L$(ULXMLRPC)/lib
LDFLAGS += -lulxmlrpcpp
endif


EXES=powerapi snippet dummyTest
all:  ${EXES}

powerapi: powerapi.c
	$(CC) ${LDFLAGS} powerapi.c -o $@ 

snippet: snippet.c
	$(CC) ${LDFLAGS} snippet.c -o $@ 

dummyTest: dummyTest.c
	$(CC) ${LDFLAGS} dummyTest.c -o $@ 

clean:
	rm -f  ${EXES}
