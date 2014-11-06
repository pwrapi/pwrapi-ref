XMLRPC = n
DEBUG = n

CFLAGS = -Wall -fPIC
CPPFLAGS = -I../tinyxml
CXXFLAGS = -Wall -fPIC
LDFLAGS = -shared
LDLIBS = -ldl
LD = g++

ifeq ($(DEBUG),y)
CPPFLAGS += -DDEBUG_ON
endif

LIB = libpow.so

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

XML_OBJS=../tinyxml/tinyxml2.o

DEPS = $(LIB_OBJS:.o=.d) 

all: ${LIB}

${LIB}: ${LIB_OBJS} ${XML_OBJS}
	$(LD) $(LDFLAGS) $(LDLIBS) -o $@ ${LIB_OBJS} ${XML_OBJS}

clean:
	rm -f test ${LIB} ${LIB_OBJS} ${XML_OBJS} $(DEPS)

-include $(DEPS) 

%.o: %.cc
	g++ -MM $(CPPFLAGS) $(CXXFLAGS) $*.cc > $*.d
	g++ $(CPPFLAGS) $(CXXFLAGS) -c $*.cc -o $*.o

%.o: %.cpp
	g++ -MM $(CPPFLAGS) $(CXXFLAGS) $*.cpp > $*.d
	g++ $(CPPFLAGS) $(CXXFLAGS) -c $*.cpp -o $*.o
