
LIB_SRC=pli.cc init.cc

LIB_OBJS = ${LIB_SRC:.cc=.o}

LIB = libpli.a

test: ${LIB}
	g++ -I. -L. main.c -lpli -o test

${LIB}: ${LIB_OBJS}
	${AR} rcs $@ ${LIB_OBJS} 

clean:
	rm -f test ${LIB} ${LIB_OBJS}

#g++ -Wall -fPIC -c pli.cc
#g++ -Wall -fPIC -c init.cc 

#gcc -shared -o libpli.so  pli.o init.o -lstdc++

#gcc -I. main.c libpli.so 
