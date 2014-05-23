
LIB_SRC=pow.cc init.cc cntxt.cc object.cc attribute.cc

LIB_OBJS = ${LIB_SRC:.cc=.o}

LIB = libpow.a

CFLAGS = -Wall 
CPPFLAGS = -Itinyxml
CXXFLAGS = -Wall

test: ${LIB} main2.c 
	$(CXX) -Wall -I. -L. tinyxml/tinyxml2.cpp main2.c -lpow -o test

${LIB}: ${LIB_OBJS}
	$(AR) rcs $@ ${LIB_OBJS} 

clean:
	rm -f test ${LIB} ${LIB_OBJS}

#g++ -Wall -fPIC -c pow.cc
#g++ -Wall -fPIC -c init.cc 

#gcc -shared -o libpow.so  pow.o init.o -lstdc++

#gcc -I. main.c libpow.so 
