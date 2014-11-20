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


ROOT=${PWD}/..

ULXMLRCP= ${HOME}/ulxmlrpcpp-1.7.5/install


LDLIBS = -lpow -ldl -lulxmlrpcpp
CPPFLAGS = -I ${ULXMLRCP}/include  -I../pow
CFLAGS = -Wall 
LDFLAGS = -L${ROOT}/pow  -L ${ULXMLRCP}/lib
CXXFLAGS = -Wall -I

all: test

test: server.cpp
	$(CXX) -Wall $(CPPFLAGS) $(LDFLAGS) server.cpp $(LDLIBS)  -o server 

clean:
	rm -f server 
