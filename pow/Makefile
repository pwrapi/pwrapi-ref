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

XMLRPC = n
DEBUG = n

CFLAGS = -Wall -fPIC
CPPFLAGS = -Itinyxml
CXXFLAGS = -Wall -fPIC
LDFLAGS = -shared
LDLIBS = -ldl
LD = g++

ifeq ($(DEBUG),y)
CPPFLAGS += -DDEBUG_ON
endif

LIB = libpow.so

LIB_SRC= debug.cc pow.cc cntxt.cc treeNode.cc deviceStat.cc xmlConfig.cc

ifeq ($(XMLRPC),y)
LIB_SRC += rpcTreeNode.cc
ULXMLRPC=$(HOME)/ulxmlrpcpp-1.7.5/install
CPPFLAGS += -DUSE_ULXMLRPC -I$(ULXMLRPC)/include
CPPFLAGS += -DUSE_RPC
LDFLAGS += -L$(ULXMLRPC)/lib
LDLIBS += -lulxmlrpcpp
endif

LIB_OBJS = ${LIB_SRC:.cc=.o}

XML_OBJS=tinyxml/tinyxml2.o

DEPS = $(LIB_OBJS:.o=.d) 

all: ${LIB}

${LIB}: ${LIB_OBJS} ${XML_OBJS}
	$(LD) $(LDFLAGS) $(LDLIBS) -o $@ ${LIB_OBJS} ${XML_OBJS}

clean:
	rm -f test ${LIB} ${LIB_OBJS} ${XML_OBJS} $(DEPS)

-include $(DEPS) 

%.o: %.cc
	g++ -MM $(CPPFLAGS) $(CXXFLAGS) $*.cc > $*.d
	g++ $(CPPFLAGS) $(CXXFLAGS) -c $*.cc -o $*.o

%.o: %.cpp
	g++ -MM $(CPPFLAGS) $(CXXFLAGS) $*.cpp > $*.d
	g++ $(CPPFLAGS) $(CXXFLAGS) -c $*.cpp -o $*.o
