
#include <stdlib.h>
#include <stdio.h>
#include "./dev.h"


typedef struct {
    float value[PWR_ATTR_INVALID];  
} dummyDevInfo_t;


static pwr_dev_t open( const char *initstr )
{
    printf("dummyDev::%s()\n",__func__);
    dummyDevInfo_t *tmp = malloc( sizeof( dummyDevInfo_t ) );
    tmp->value[PWR_ATTR_POWER] = 10.1234;
    return tmp;
}

static int close( pwr_dev_t dev)
{
    printf("dummyDev::%s()\n",__func__);
    free( dev );
}

static int read( pwr_dev_t dev, PWR_AttrType type, void* ptr, size_t len, PWR_Time* ts )
{
    printf("dummyDev::%s()\n",__func__);

    *(float*)ptr = ((dummyDevInfo_t*) dev)->value[type];

    struct timeval tv;
    gettimeofday(&tv,NULL);

    if ( ts ) {
        *ts = tv.tv_sec * 1000000000;
        *ts += tv.tv_usec * 1000;
    }

    return PWR_ERR_SUCCESS;
}

static int write( pwr_dev_t dev, PWR_AttrType type, void* ptr, size_t len )
{
    printf("dummyDev::%s()\n",__func__);
    ((dummyDevInfo_t*) dev)->value[type] = *(float*)ptr;
    return PWR_ERR_SUCCESS;
}

static int readv( pwr_dev_t dev, unsigned int arraysize, PWR_Value values[], int status[] )
{
    int i;
    printf("dummyDev::%s()\n",__func__);
    for ( i = 0; i < arraysize; i++ ) {
        printf("dummyDev::%s() %d\n",__func__, values[i].type);
        *(float*)values[i].ptr = ((dummyDevInfo_t*) dev)->value[values[i].type];

        struct timeval tv;
        gettimeofday(&tv,NULL);

        values[i].timeStamp = tv.tv_sec * 1000000000;
        values[i].timeStamp += tv.tv_usec * 1000;

        status[i] = PWR_ERR_SUCCESS;
    }
}

static int writev( pwr_dev_t dev, unsigned int arraysize, PWR_Value values[], int status[] )
{
    int i;
    printf("dummyDev::%s()\n",__func__);
    for ( i = 0; i < arraysize; i++ ) {
        printf("dummyDev::%s() %d %f\n",__func__, values[i].type, *(float*) values[i].ptr);
        ((dummyDevInfo_t*) dev)->value[values[i].type] = *(float*)values[i].ptr;

        status[i] = PWR_ERR_SUCCESS;
    }
}

static int time( pwr_dev_t dev, unsigned long long *timestamp )
{
    printf("dummyDev::%s()\n",__func__);
}


static plugin_dev_t dev = {
    open: open, 
    close: close,
    read: read,
    write: write,
    readv: readv,
    writev: writev,
    time: time,
};

plugin_dev_t* getDev() {
    return &dev;
}
