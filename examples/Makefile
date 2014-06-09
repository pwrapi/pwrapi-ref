CFLAGS = -Wall -I.. -fPIC
CXXFLAGS = ${CFLAGS}
LDFLAGS = ${CFLAGS} -L/home/ddeboni/src/prototype -L/home/ddeboni/src/prototype/plugins -lpow -lm -ldl -pthread

all:  example

example: main.c
	$(CC) ${LDFLAGS} main.c -o example

clean:
	rm -f example

