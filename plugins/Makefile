LIB_SRC=mchw_pidev.c mchw_rapldev.c mchw_xtpmdev.c

LIB_OBJS = ${LIB_SRC:.c=.o}

LIB = libplugins.a

CFLAGS = -Wall -I..
CXXFLAGS = ${CFLAGS}

all:  test

test: ${LIB} main.c
	$(CC) -Wall -I.. -L. -lplugins -lpiapi main.c -o test

${LIB}: ${LIB_OBJS}
	$(AR) rcs $@ ${LIB_OBJS} 

clean:
	rm -f ${LIB} ${LIB_OBJS} test

