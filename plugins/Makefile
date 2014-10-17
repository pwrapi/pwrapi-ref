MACOS=
PI=

CC=gcc
CFLAGS=-Wall -I.. -fPIC

CXXFLAGS=${CFLAGS}
LDFLAGS=${CFLAGS} -L. -lpiapi -lm -pthread

all:  test

test: libdummy_dev.so libpwr_pidev.so libpwr_rapldev.so libpwr_xtpmdev.so libpwr_wudev.so libpwr_pgdev.so libpwr_cpudev.so test.c
	$(CC) ${LDFLAGS} test.c -lpwr_rapldev -lpwr_xtpmdev -lpwr_wudev -lpwr_cpudev -o test

libdummy_dev.so: pwr_dev.o dummy_dev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o dummy_dev.o

ifeq ($(MACOS),y)
libpwr_pgdev.so: pwr_dev.o pwr_pgdev.o
	$(CC) -shared $(CFLAGS) -framework IntelPowerGadget -o $@ pwr_dev.o pwr_pgdev.o
else
libpwr_pgdev.so:
endif

ifeq ($(PI),y)
libpwr_pidev.so: pwr_dev.o pwr_pidev.o
	$(CC) -shared $(CFLAGS) -lpiapi -o $@ pwr_dev.o pwr_pidev.o
else
libpwr_pidev.so:
endif

libpwr_rapldev.so: pwr_dev.o pwr_rapldev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o pwr_rapldev.o

libpwr_xtpmdev.so: pwr_dev.o pwr_xtpmdev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o pwr_xtpmdev.o

libpwr_wudev.so: pwr_dev.o pwr_wudev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o pwr_wudev.o

libpwr_cpudev.so: pwr_dev.o pwr_cpudev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o pwr_cpudev.o

clean:
	rm -f *_*dev.o lib*_*dev.so test

