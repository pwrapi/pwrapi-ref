CC = gcc
CFLAGS = -Wall -I.. -fPIC
CXXFLAGS = ${CFLAGS}
LDFLAGS = ${CFLAGS} -L. -lpiapi -lm -pthread

all:  test

test: libdummy_dev.so libpwr_pidev.so libpwr_rapldev.so libpwr_xtpmdev.so libpwr_wudev.so libpwr_cpudev.so main.c
	$(CC) ${LDFLAGS} main.c -lpwr_pidev -lpwr_rapldev -lpwr_xtpmdev -lpwr_wudev -lpwr_cpudev -o test

libdummy_dev.so: pwr_dev.o dummy_dev.o
	$(CC) -shared $(CFLAGS) -o $@ pwr_dev.o dummy_dev.o

libpwr_pidev.so: pwr_dev.o pwr_pidev.o
	$(CC) -shared $(CFLAGS) -lpiapi -o $@ pwr_dev.o pwr_pidev.o

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

