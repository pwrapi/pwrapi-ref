
LIB_SRC=pow.cc init.cc

LIB_OBJS = ${LIB_SRC:.cc=.o}

LIB = libpow.a

test: ${LIB} main.c
	g++ -I. -L. main.c -lpow -o test

${LIB}: ${LIB_OBJS}
	${AR} rcs $@ ${LIB_OBJS} 

clean:
	rm -f test ${LIB} ${LIB_OBJS}

#g++ -Wall -fPIC -c pow.cc
#g++ -Wall -fPIC -c init.cc 

#gcc -shared -o libpow.so  pow.o init.o -lstdc++

#gcc -I. main.c libpow.so 
