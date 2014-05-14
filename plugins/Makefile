LIB_INC=..
LIB_SRC=mchw_pidev.c mchw_rapldev.c mchw_xtpmdev.c

LIB_OBJS = ${LIB_SRC:.c=.o}

LIB = libplugins.a

CFLAGS = -Wall -I${LIB_INC}
CXXFLAGS = ${CFLAGS}

${LIB}: ${LIB_OBJS}
	$(AR) rcs $@ ${LIB_OBJS} 

clean:
	rm -f ${LIB} ${LIB_OBJS}

