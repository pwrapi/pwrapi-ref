CFLAGS = -Wall -I.. -fPIC
CXXFLAGS = ${CFLAGS}
LDFLAGS = ${CFLAGS} -L.. -L../plugins -lpow -lm -ldl -pthread

all:  powerapi

powerapi: powerapi.c
	$(CC) ${LDFLAGS} powerapi.c -o powerapi

clean:
	rm -f powerapi

