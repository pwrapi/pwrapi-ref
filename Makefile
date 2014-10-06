
LIB = libpow.so

CFLAGS = -Wall 
CXXFLAGS = -Wall

LDFLAGS = -L.

LDLIBS = -lpow 

test: ${LIB} main.c 
	$(CC) -Wall -I. $(LDFLAGS) main.c $(LDLIBS) -o test

clean:
	rm -f test 
