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
