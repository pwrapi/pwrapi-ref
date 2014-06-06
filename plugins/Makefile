CFLAGS = -Wall -I.. -fPIC
CXXFLAGS = ${CFLAGS}
LDFLAGS = ${CFLAGS} -L. -lpiapi -lm -pthread

all:  test

test: libmchw_pidev.so libmchw_rapldev.so libmchw_xtpmdev.so liboshw_cpudev.so main.c
	$(CC) ${LDFLAGS} main.c -lmchw_pidev -lmchw_rapldev -lmchw_xtpmdev -loshw_cpudev -o test

libmchw_pidev.so: mchw_pidev.o
	$(CC) -shared $(CFLAGS) -lpiapi -o $@ mchw_pidev.o

libmchw_rapldev.so: mchw_rapldev.o
	$(CC) -shared $(CFLAGS) -o $@ mchw_rapldev.o

libmchw_xtpmdev.so: mchw_xtpmdev.o
	$(CC) -shared $(CFLAGS) -o $@ mchw_xtpmdev.o

liboshw_cpudev.so: oshw_cpudev.o
	$(CC) -shared $(CFLAGS) -o $@ oshw_cpudev.o

clean:
	rm -f *hw_*dev.o lib*hw_*dev.so test

