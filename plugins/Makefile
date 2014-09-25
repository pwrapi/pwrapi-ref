CC = gcc
CFLAGS = -Wall -I.. -fPIC
CXXFLAGS = ${CFLAGS}
LDFLAGS = ${CFLAGS} -L. -lpiapi -lm -pthread

all:  test

test: libdummy_dev.so libmchw_pidev.so libmchw_rapldev.so libmchw_xtpmdev.so libmchw_wudev.so liboshw_cpudev.so main.c
	$(CC) ${LDFLAGS} main.c -lmchw_pidev -lmchw_rapldev -lmchw_xtpmdev -lmchw_wudev -loshw_cpudev -o test

libdummy_dev.so: dummy_dev.o
	$(CC) -shared $(CFLAGS) -o $@ dummy_dev.o

libmchw_pidev.so: mchw_pidev.o
	$(CC) -shared $(CFLAGS) -lpiapi -o $@ mchw_pidev.o

libmchw_rapldev.so: mchw_rapldev.o
	$(CC) -shared $(CFLAGS) -o $@ mchw_rapldev.o

libmchw_xtpmdev.so: mchw_xtpmdev.o
	$(CC) -shared $(CFLAGS) -o $@ mchw_xtpmdev.o

libmchw_wudev.so: mchw_wudev.o
	$(CC) -shared $(CFLAGS) -o $@ mchw_wudev.o

liboshw_cpudev.so: oshw_cpudev.o
	$(CC) -shared $(CFLAGS) -o $@ oshw_cpudev.o

clean:
	rm -f *_*dev.o lib*_*dev.so test

