
LIB = ../libpow.so

ULXMLRCP= /home/mjleven/ulxmlrpcpp-1.7.5/install

#-DHAVE_CONFIG_H -DULXR_HAVE_CONFIG_H -I /home/mjleven/ulxmlrpcpp-1.7.5/ -I /home/mjleven/ulxmlrpcpp-1.7.5/install/include server.cpp -L 
#-l ulxmlrpcpp

LDLIBS = -lpow -ldl -lulxmlrpcpp
CPPFLAGS = -I ${ULXMLRCP}/include 
CFLAGS = -Wall 
LDFLAGS = -L..  -L ${ULXMLRCP}/lib
CXXFLAGS = -Wall -I

test: ${LIB} server.cpp
	$(CXX) -Wall $(CPPFLAGS) $(LDFLAGS) server.cpp $(LDLIBS)  -o server 

clean:
	rm -f server 
