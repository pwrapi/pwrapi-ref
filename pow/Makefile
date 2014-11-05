XMLRPC = n
SWIG = n

CFLAGS = -Wall -fPIC
CPPFLAGS = -I../tinyxml #-DDEBUG_ON
CPPFLAGS += -I/usr/include/python2.7
CXXFLAGS = -Wall -fPIC
LDFLAGS = -shared
LDFLAGS += $(shell python-config --ldflags)
LDLIBS = -ldl
CC = g++

LIB_SRC= debug.cc pow.cc cntxt.cc treeNode.cc deviceStat.cc xmlConfig.cc

ifeq ($(XMLRPC),y)
LIB_SRC += rpcTreeNode.cc
ULXMLRPC=$(HOME)/ulxmlrpcpp-1.7.5/install
CPPFLAGS += -DUSE_ULXMLRPC -I$(ULXMLRPC)/include
CPPFLAGS += -DUSE_RPC
LDFLAGS += -L$(ULXMLRPC)/lib
LDLIBS += -lulxmlrpcpp
endif

LIB_OBJS = ${LIB_SRC:.cc=.o}

LIB = libpow.so

XML_OBJS=../tinyxml/tinyxml2.o

DEPS = $(LIB_OBJS:.o=.d) 

all: ${LIB}

ifeq ($(SWIG),y)
LIB_OBJS += pow_wrap.o

pow_wrap.c: pow.i
	swig -classic -python pow.i

endif

${LIB}: ${LIB_OBJS} ${XML_OBJS}
	$(CC) $(LDFLAGS) $(CFLAGS) $(LDLIBS) -o $@ ${LIB_OBJS} ${XML_OBJS}

clean:
	rm -f test ${LIB} ${LIB_OBJS} ${XML_OBJS} $(DEPS) pow_wrap.c pow.py pow.pyc

-include $(DEPS) 

%.o: %.cc
	g++ -MM $(CPPFLAGS) $(CXXFLAGS) $*.cc > $*.d
	g++ $(CPPFLAGS) $(CXXFLAGS) -c $*.cc -o $*.o

%.o: %.cpp
	g++ -MM $(CPPFLAGS) $(CXXFLAGS) $*.cpp > $*.d
	g++ $(CPPFLAGS) $(CXXFLAGS) -c $*.cpp -o $*.o
