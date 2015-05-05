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
DEBUG ?= n
SHARED ?= y

LD = g++
CXXFLAGS = -Wall
CPPFLAGS = -I../tinyxml

LIB_SRC= debug.cc pwr.cc cntxt.cc treeNode.cc deviceStat.cc xmlConfig.cc

ifeq ($(SHARED),y)
CXXFLAGS += -fPIC
LDFLAGS += -shared
LDLIBS += -ldl
LIB = libpwr.so
LIB_SRC += dynamic.cc
else
LIB = libpwr.a
CPPFLAGS += -DHAVE_POWER_INSIGHT
LIB_SRC += static.cc
endif


ifeq ($(DEBUG),y)
CPPFLAGS += -DDEBUG_ON -g
endif



ifeq ($(XMLRPC),y)
LIB_SRC += rpcTreeNode.cc
ULXMLRPC=$(HOME)/ulxmlrpcpp-1.7.5/install
CPPFLAGS += -DUSE_ULXMLRPC -I$(ULXMLRPC)/include
CPPFLAGS += -DUSE_RPC
LDFLAGS += -L$(ULXMLRPC)/lib
LDLIBS += -lulxmlrpcpp
endif

LIB_OBJS = ${LIB_SRC:.cc=.o}

XML_OBJS=../tinyxml/tinyxml2.o

DEPS = $(LIB_OBJS:.o=.d) 

all: ${LIB}

${LIB}: ${LIB_OBJS} ${XML_OBJS}
ifeq ($(SHARED),y)
	$(LD) $(LDFLAGS) $(LDLIBS) -o $@ ${LIB_OBJS} ${XML_OBJS}
else
	ar rcs $@ ${LIB_OBJS} ${XML_OBJS}
endif

clean:
	rm -f test ${LIB} ${LIB_OBJS} ${XML_OBJS} $(DEPS)

-include $(DEPS) 

%.o: %.cc
	g++ -MM $(CPPFLAGS) $(CXXFLAGS) $*.cc > $*.d
	g++ $(CPPFLAGS) $(CXXFLAGS) -c $*.cc -o $*.o

%.o: %.cpp
	g++ -MM $(CPPFLAGS) $(CXXFLAGS) $*.cpp > $*.d
	g++ $(CPPFLAGS) $(CXXFLAGS) -c $*.cpp -o $*.o
