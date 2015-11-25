
PWR=$(HOME)/pwrGIT/dist/build/install
CFLAGS = -Wall 
CPPFLAGS = -Wall  -I$(PWR)/include -I../include -DNO_CONFIG -DUSE_DEBUG
LDFLAGS= -L $(PWR)/lib

all: server 

server:  server.cc allocEvent.cc main.cc
	g++ $(CPPFLAGS) $(LDFLAGS) server.cc allocEvent.cc main.cc -lpwr  -o $@

clean:
	rm -f  *.o server
