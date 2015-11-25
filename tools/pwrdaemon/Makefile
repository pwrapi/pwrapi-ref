PWR=$(HOME)/pwrGIT/dist/build/install
CFLAGS = -Wall 
CPPFLAGS = -Wall -I$(PWR)/include -Irouter -Iserver -I../include -DNO_CONFIG -DUSE_DEBUG
LDFLAGS= -L $(PWR)/lib

RTR_SRC= \
	 router/router.cc \
	 router/routerSelect.cc \
	 router/allocEvent.cc \
	 router/client.cc \
	 router/commCreateEvent.cc

SRVR_SRC= \
	server/server.cc \
	server/allocEvent.cc

COMB_SRC =	$(SRVR_SRC) $(RTR_SRC)

RTR_EXEC=rtrMain
SRVR_EXEC=srvrMain
COMB_EXEC=combMain

EXECS=$(RTR_EXEC) $(SRVR_EXEC) $(COMB_EXEC)

all: $(EXECS)

$(RTR_EXEC): $(RTR_SRC) rtrMain.cc
	g++ $(CPPFLAGS) $(LDFLAGS) $(RTR_SRC) rtrMain.cc -lpwr  -o $@

$(SRVR_EXEC): $(SRVR_SRC) srvrMain.cc
	g++ $(CPPFLAGS) $(LDFLAGS) $(SRVR_SRC) srvrMain.cc -lpwr  -o $@

$(COMB_EXEC): $(COMB_SRC) combMain.cc
	g++ $(CPPFLAGS) $(LDFLAGS) $(COMB_SRC) combMain.cc -lpwr  -o $@


clean:
	rm -f  *.o $(EXECS) 
