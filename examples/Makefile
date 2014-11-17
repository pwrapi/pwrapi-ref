CC=gcc
CFLAGS = -Wall -I../pow -fPIC
CXXFLAGS = ${CFLAGS}
LDFLAGS = ${CFLAGS} -L../pow -lpow -lm -ldl -pthread

EXES=powerapi snippet dummyTest
all:  ${EXES}

powerapi: powerapi.c
	$(CC) ${LDFLAGS} powerapi.c -o $@ 

snippet: snippet.c
	$(CC) ${LDFLAGS} snippet.c -o $@ 

dummyTest: dummyTest.c
	$(CC) ${LDFLAGS} dummyTest.c -o $@ 

clean:
	rm -f  ${EXES}
