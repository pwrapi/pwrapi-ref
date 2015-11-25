PWR=$(HOME)/pwrGIT/dist/build/install
CFLAGS = -Wall 
CPPFLAGS = -Wall -I$(PWR)/include -I../include -DNO_CONFIG -DUSE_DEBUG
LDFLAGS= -L $(PWR)/lib

_SRC=main.cc router.cc routerSelect.cc allocEvent.cc client.cc commCreateEvent.cc

all: router 

router: $(_SRC) router.h
	g++ $(CPPFLAGS) $(LDFLAGS) $(_SRC) -lpwr  -o $@

clean:
	rm -f  *.o router 
