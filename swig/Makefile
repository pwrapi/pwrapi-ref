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
CPPFLAGS = -I../pow -I/usr/include/python2.7
LDFLAGS = -shared $(shell python-config --ldflags) -L../pow
LD = ld 

all: _pow.so

_pow.so: pow_wrap.o
	$(CC) $(LDFLAGS) -L. -lpow $< -o $@  

pow_wrap.c: pow.i ../pow/pow.h ../pow/types.h
	swig -I../pow -classic -python pow.i

clean:
	rm -f _pow.so pow.py pow.pyc pow_wrap.c pow_wrap.o
