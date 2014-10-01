
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
    char config[100];
} dummyDevInfo_t;

typedef struct {
    double value[PWR_ATTR_INVALID];
} dummyFdInfo_t;
  
static pwr_fd_t dummy_dev_open( plugin_devOps_t* ops, const char *openstr )
{
    DBGX("`%s`\n",openstr);
    dummyFdInfo_t *tmp = malloc( sizeof( dummyFdInfo_t ) );
    tmp->value[PWR_ATTR_POWER] = 10.1234;
    return tmp;
}

static int dummy_dev_close( pwr_fd_t fd )
{
    DBGX("\n");
    free( fd );

    return 0;
}

static int dummy_dev_read( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len, PWR_Time* ts )
{

    *(double*)ptr = ((dummyFdInfo_t*) fd)->value[type];

    DBGX("type=%s %f\n", attrNameToString(type),*(double*)ptr);

    struct timeval tv;
    gettimeofday(&tv,NULL);

    if ( ts ) {
        *ts = tv.tv_sec * 1000000000;
        *ts += tv.tv_usec * 1000;
    }

    return PWR_RET_SUCCESS;
}

static int dummy_dev_write( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len )
{
    DBGX("type=%s %f\n",attrNameToString(type), *(double*)ptr);

    ((dummyFdInfo_t*) fd)->value[type] = *(double*)ptr;
    return PWR_RET_SUCCESS;
}

static int dummy_dev_readv( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], void* buf,
                        PWR_Time ts[], int status[] )
{
    int i;
    for ( i = 0; i < arraysize; i++ ) {

        ((double*)buf)[i] = ((dummyFdInfo_t*) fd)->value[attrs[i]];

        DBGX("type=%s %f\n",attrNameToString(attrs[i]), ((double*)buf)[i]);

        struct timeval tv;
        gettimeofday(&tv,NULL);

        ts[i] = tv.tv_sec * 1000000000;
        ts[i] += tv.tv_usec * 1000;

        status[i] = PWR_RET_SUCCESS;
    }
    return PWR_RET_SUCCESS;
}

static int dummy_dev_writev( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], void* buf, int status[] )
{
    int i;
    DBGX("num attributes %d\n",arraysize);
    for ( i = 0; i < arraysize; i++ ) {
        DBGX("type=%s %f\n",attrNameToString(attrs[i]), ((double*)buf)[i]);

        ((dummyFdInfo_t*) fd)->value[attrs[i]] = ((double*)buf)[i];

        status[i] = PWR_RET_SUCCESS;
    }
    return PWR_RET_SUCCESS;
}

static int dummy_dev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    DBGX("\n");

    return 0;
}

static int dummy_dev_clear( pwr_fd_t fd )
{
    DBGX("\n");

    return 0;
}

static plugin_devOps_t devOps = {
    .open   = dummy_dev_open, 
    .close  = dummy_dev_close,
    .read   = dummy_dev_read,
    .write  = dummy_dev_write,
    .readv  = dummy_dev_readv,
    .writev = dummy_dev_writev,
    .time   = dummy_dev_time,
    .clear  = dummy_dev_clear
};

static plugin_devOps_t* dummy_dev_init( const char *initstr )
{
	plugin_devOps_t* ops = malloc(sizeof(*ops));
	*ops = devOps;
	ops->privateData = malloc( sizeof( dummyDevInfo_t ) );
    return ops;
}

static int dummy_dev_final( plugin_devOps_t *ops )
{
    DBGX("\n");
	free( ops->privateData );
    free( ops );
    return 0;
}

static plugin_dev_t dev = {
    .init   = dummy_dev_init, 
    .final  = dummy_dev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}
