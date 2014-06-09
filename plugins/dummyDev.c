
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./dev.h"
#include "../util.h"


#if 1

#define DBGX( fmt, args... ) \
{\
    fprintf( stderr, "DummyDev:%s():%d: "fmt, __func__, __LINE__, ##args);\
}

#else

#define DBGX( fmt, args... )

#endif


typedef struct {
    float value[PWR_ATTR_INVALID];  
    char config[100];
} dummyDevInfo_t;


static pwr_dev_t open( const char *initstr )
{
    DBGX("`%s`\n",initstr);
    dummyDevInfo_t *tmp = malloc( sizeof( dummyDevInfo_t ) );
    tmp->value[PWR_ATTR_POWER] = 10.1234;
    strcpy( tmp->config, initstr );
    return tmp;
}

static int close( pwr_dev_t dev)
{
    DBGX("\n");
    free( dev );
}

static int read( pwr_dev_t dev, PWR_AttrType type, void* ptr, size_t len, unsigned long long* ts )
{

    *(float*)ptr = ((dummyDevInfo_t*) dev)->value[type];

    DBGX("type=%s %f\n",attrTypeToString(type),*(float*)ptr);

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
    DBGX("type=%s %f\n",attrTypeToString(type), *(float*)ptr);

    ((dummyDevInfo_t*) dev)->value[type] = *(float*)ptr;
    return PWR_ERR_SUCCESS;
}

static int readv( pwr_dev_t dev, unsigned int arraysize, const PWR_AttrType attrs[], void* buf,
                        unsigned long long ts[], int status[] )
{
    int i;
    for ( i = 0; i < arraysize; i++ ) {

        ((float*)buf)[i] = ((dummyDevInfo_t*) dev)->value[attrs[i]];

        DBGX("type=%s %f\n",attrTypeToString(attrs[i]), ((float*)buf)[i]);

        struct timeval tv;
        gettimeofday(&tv,NULL);

        ts[i] = tv.tv_sec * 1000000000;
        ts[i] += tv.tv_usec * 1000;

        status[i] = PWR_ERR_SUCCESS;
    }
    return PWR_ERR_SUCCESS;
}

static int writev( pwr_dev_t dev, unsigned int arraysize, const PWR_AttrType attrs[], void* buf, int status[] )
{
    int i;
    DBGX("num attributes %d\n",arraysize);
    for ( i = 0; i < arraysize; i++ ) {
        DBGX("type=%s %f\n",attrTypeToString(attrs[i]), ((float*)buf)[i]);

        ((dummyDevInfo_t*) dev)->value[attrs[i]] = ((float*)buf)[i];

        status[i] = PWR_ERR_SUCCESS;
    }
    return PWR_ERR_SUCCESS;
}

static int time( pwr_dev_t dev, unsigned long long *timestamp )
{
    DBGX("\n");
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
