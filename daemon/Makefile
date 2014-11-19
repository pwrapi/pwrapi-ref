ROOT=${PWD}/..

ULXMLRCP= /home/mjleven/ulxmlrpcpp-1.7.5/install

#-DHAVE_CONFIG_H -DULXR_HAVE_CONFIG_H -I /home/mjleven/ulxmlrpcpp-1.7.5/ -I /home/mjleven/ulxmlrpcpp-1.7.5/install/include server.cpp -L 
#-l ulxmlrpcpp

LDLIBS = -lpow -ldl -lulxmlrpcpp
CPPFLAGS = -I ${ULXMLRCP}/include  -I../pow
CFLAGS = -Wall 
LDFLAGS = -L${ROOT}/pow  -L ${ULXMLRCP}/lib
CXXFLAGS = -Wall -I

all: test

test: server.cpp
	$(CXX) -Wall $(CPPFLAGS) $(LDFLAGS) server.cpp $(LDLIBS)  -o server 

clean:
	rm -f server 
