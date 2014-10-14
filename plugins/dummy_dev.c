
#include <assert.h>
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

#define BUFFER_LEN 10

typedef struct {
	double values[BUFFER_LEN];
	PWR_Time timeStamps[BUFFER_LEN];
} buffer_t;

typedef struct {
	buffer_t buffers[PWR_ATTR_INVALID];
} dummyFdInfo_t;
  
static pwr_fd_t dummy_dev_open( plugin_devops_t* ops, const char *openstr )
{
    DBGX("`%s`\n",openstr);
    dummyFdInfo_t *tmp = malloc( sizeof( dummyFdInfo_t ) );
    tmp->buffers[PWR_ATTR_POWER].values[0] = 10.1234;
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

    *(double*)ptr = ((dummyFdInfo_t*) fd)->buffers[type].values[0];

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

    ((dummyFdInfo_t*) fd)->buffers[type].values[0] = *(double*)ptr;
    return PWR_RET_SUCCESS;
}

static int dummy_dev_readv( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], void* buf,
                        PWR_Time ts[], int status[] )
{
    int i;
    for ( i = 0; i < arraysize; i++ ) {

        ((double*)buf)[i] = ((dummyFdInfo_t*) fd)->buffers[attrs[i]].values[0];

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

        ((dummyFdInfo_t*) fd)->buffers[attrs[i]].values[0] = ((double*)buf)[i];

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

static int dummy_dev_get_stat( pwr_fd_t fd, PWR_AttrName name, statOp_t op,
                                    void* result, PWR_StatTimes* ts )
{
    buffer_t* ptr = &((dummyFdInfo_t*) fd)->buffers[name];
    DBGX("\n");
	return  op(BUFFER_LEN, ptr->values,result,&ts->instant);
}

static plugin_devops_t devOps = {
    .open   = dummy_dev_open, 
    .close  = dummy_dev_close,
    .read   = dummy_dev_read,
    .write  = dummy_dev_write,
    .readv  = dummy_dev_readv,
    .writev = dummy_dev_writev,
    .time   = dummy_dev_time,
    .clear  = dummy_dev_clear,
	.get_stat = dummy_dev_get_stat,
};

static plugin_devops_t* dummy_dev_init( const char *initstr )
{
	plugin_devops_t* ops = malloc(sizeof(*ops));
	*ops = devOps;
	ops->private_data = malloc( sizeof( dummyDevInfo_t ) );
    return ops;
}

static int dummy_dev_final( plugin_devops_t *ops )
{
    DBGX("\n");
	free( ops->private_data );
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
