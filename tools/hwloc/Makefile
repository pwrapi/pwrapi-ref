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

CXXFLAGS = -Wall -I../../pow -I../../tinyxml -fPIC

CXX = g++
XML_OBJS=../../tinyxml/tinyxml2.o

all:  hwloc-pow

hwloc-pow: hwloc.cc ${XML_OBJS}
	$(CXX) ${CXXFLAGS} $^ -o $@

clean:
	rm -f hwloc-pow hwloc.xml
