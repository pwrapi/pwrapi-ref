
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "dev.h"
#include "util.h"

#if 1

#define DBGX( fmt, args... ) \
{\
    fprintf( stderr, "DummyDev:%s():%d: "fmt, __func__, __LINE__, ##args);\
}

#else

#define DBGX( fmt, args... )

#endif


typedef struct {
    double value[PWR_ATTR_INVALID];  
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

    return 0;
}

static int read( pwr_dev_t dev, PWR_AttrName type, void* ptr, unsigned int len, PWR_Time* ts )
{

    *(double*)ptr = ((dummyDevInfo_t*) dev)->value[type];

    DBGX("type=%s %f\n",attrNameToString(type),*(double*)ptr);

    struct timeval tv;
    gettimeofday(&tv,NULL);

    if ( ts ) {
        *ts = tv.tv_sec * 1000000000;
        *ts += tv.tv_usec * 1000;
    }

    return PWR_ERR_SUCCESS;
}

static int write( pwr_dev_t dev, PWR_AttrName type, void* ptr, unsigned int len )
{
    DBGX("type=%s %f\n",attrNameToString(type), *(double*)ptr);

    ((dummyDevInfo_t*) dev)->value[type] = *(double*)ptr;
    return PWR_ERR_SUCCESS;
}

static int readv( pwr_dev_t dev, unsigned int arraysize, const PWR_AttrName attrs[], void* buf,
                        PWR_Time ts[], int status[] )
{
    int i;
    for ( i = 0; i < arraysize; i++ ) {

        ((double*)buf)[i] = ((dummyDevInfo_t*) dev)->value[attrs[i]];

        DBGX("type=%s %f\n",attrNameToString(attrs[i]), ((double*)buf)[i]);

        struct timeval tv;
        gettimeofday(&tv,NULL);

        ts[i] = tv.tv_sec * 1000000000;
        ts[i] += tv.tv_usec * 1000;

        status[i] = PWR_ERR_SUCCESS;
    }
    return PWR_ERR_SUCCESS;
}

static int writev( pwr_dev_t dev, unsigned int arraysize, const PWR_AttrName attrs[], void* buf, int status[] )
{
    int i;
    DBGX("num attributes %d\n",arraysize);
    for ( i = 0; i < arraysize; i++ ) {
        DBGX("type=%s %f\n",attrNameToString(attrs[i]), ((double*)buf)[i]);

        ((dummyDevInfo_t*) dev)->value[attrs[i]] = ((double*)buf)[i];

        status[i] = PWR_ERR_SUCCESS;
    }
    return PWR_ERR_SUCCESS;
}

static int _time( pwr_dev_t dev, PWR_Time *timestamp )
{
    DBGX("\n");

    return 0;
}

static plugin_dev_t dev = {
    open: open, 
    close: close,
    read: read,
    write: write,
    readv: readv,
    writev: writev,
    time: _time,
};

plugin_dev_t* getDev() {
    return &dev;
}
