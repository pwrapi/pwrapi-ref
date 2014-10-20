CFLAGS = -Wall -I../lib -fPIC
CXXFLAGS = ${CFLAGS}
LDFLAGS = ${CFLAGS} -L../lib -L../plugins -lpow -lm -ldl -pthread

EXES=powerapi dummyTest
all:  ${EXES}

powerapi: powerapi.c
	$(CC) ${LDFLAGS} powerapi.c -o $@ 

dummyTest: dummyTest.c
	$(CC) ${LDFLAGS} dummyTest.c -o $@ 

clean:
	rm -f  ${EXES}
