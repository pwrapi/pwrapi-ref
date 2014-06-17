
LIB = libpow.so

CFLAGS = -Wall 
CXXFLAGS = -Wall

LDFLAGS = -L.

LDLIBS = -lpow 

test: ${LIB} main2.c 
	$(CC) -Wall -I. $(LDFLAGS) main2.c $(LDLIBS) -o test

clean:
	rm -f test 
