CFLAGS = -Wall -I.. -fPIC
CXXFLAGS = ${CFLAGS}
LDFLAGS = ${CFLAGS} -L.. -L../plugins -lpow -lmchw_pidev -lmchw_rapldev -lmchw_xtpmdev -loshw_cpudev -lpiapi -lm -ldl -pthread

all:  example

example: main.c
	$(CC) ${LDFLAGS} main.c -o example

clean:
	rm -f example

