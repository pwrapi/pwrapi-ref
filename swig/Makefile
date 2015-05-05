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


CFLAGS = -Wall -fPIC
CPPFLAGS = -I../pwr -I/usr/include/python2.7
LDFLAGS = -shared $(shell python-config --ldflags) -L../pwr
LD = ld 

all: _pwr.so

_pwr.so: pwr_wrap.o
	$(CC) $(LDFLAGS) -L. -lpwr $< -o $@  

pwr_wrap.c: pwr.i ../pwr/pwr.h ../pwr/types.h
	swig -I../pwr -classic -python pwr.i

clean:
	rm -f _pwr.so pwr.py pwr.pyc pwr_wrap.c pwr_wrap.o
