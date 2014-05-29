
#include <stdlib.h>
#include <stdio.h>
#include "./dev.h"


typedef struct {
} dummyDevInfo_t;


static pwr_dev_t open( const char *initstr )
{
    printf("dummyDev::%s()\n",__func__);
    return malloc( sizeof( dummyDevInfo_t ) );
}

static int close( pwr_dev_t dev)
{
    printf("dummyDev::%s()\n",__func__);
    free( dev );
}

static int read( pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float reading[], unsigned long long *timestamp)
{
    int i;
    printf("dummyDev::%s()\n",__func__);
    for ( i = 0; i < arraysize; i++ ) {
        printf("dummyDev::%s() %d\n",__func__, type[i]);
        reading[i] = 10.12011;
    }
}

static int write( pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float setting[], unsigned long long *timestamp)
{
    int i;
    printf("dummyDev::%s()\n",__func__);
    for ( i = 0; i < arraysize; i++ ) {
        printf("dummyDev::%s() %d %f\n",__func__, type[i],setting[i]);
    }
}

static int time(pwr_dev_t dev, unsigned long long *time)
{
    printf("dummyDev::%s()\n",__func__);
}


static plugin_dev_t dev = {
    open: open, 
    close: close,
    read: read,
    write: write,
    time: time,
};

plugin_dev_t* getDev() {
    return &dev;
}
