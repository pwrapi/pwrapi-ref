
LIB = libpow.so

CFLAGS = -Wall 
CXXFLAGS = -Wall

test: ${LIB} main2.c 
	$(CC) -Wall -I. -L. main2.c -lpow -o test

clean:
	rm -f test 
