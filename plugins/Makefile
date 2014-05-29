LIB_SRC=mchw_pidev.c mchw_rapldev.c mchw_xtpmdev.c oshw_cpudev.c

LIB_OBJS = ${LIB_SRC:.c=.o}

LIB = libplugins.a

CFLAGS = -Wall -I..
CXXFLAGS = ${CFLAGS}

all:  test

test: ${LIB} main.c
	$(CC) -Wall -I.. main.c -L. -lplugins -lpiapi -lm -pthread -o test

${LIB}: ${LIB_OBJS}
	$(AR) rcs $@ ${LIB_OBJS} 

clean:
	rm -f ${LIB} ${LIB_OBJS} test

