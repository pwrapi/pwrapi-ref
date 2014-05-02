LIB_INC=..
LIB_SRC=mchw_pidev.cc

LIB_OBJS = ${LIB_SRC:.cc=.o}

LIB = libplugins.a

CFLAGS = -Wall -I${LIB_INC}
CXXFLAGS = ${CFLAGS}

${LIB}: ${LIB_OBJS}
	$(AR) rcs $@ ${LIB_OBJS} 

clean:
	rm -f ${LIB} ${LIB_OBJS}

