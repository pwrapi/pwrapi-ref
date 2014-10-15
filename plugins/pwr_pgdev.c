#include "pwr_pgdev.h"
#include "pwr_dev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include <IntelPowerGadget/EnergyLib.h>

static int pgdev_verbose = 0;

typedef struct {
    int num_msrs;
} pwr_pgdev_t;
#define PWR_PGDEV(X) ((pwr_pgdev_t *)(X))

typedef struct {
    pwr_pgdev_t *dev;
} pwr_pgfd_t;
#define PWR_PGFD(X) ((pwr_pgfd_t *)(X))

static plugin_devops_t devops = {
    .open         = pwr_pgdev_open,
    .close        = pwr_pgdev_close,
    .read         = pwr_pgdev_read,
    .write        = pwr_pgdev_write,
    .readv        = pwr_pgdev_readv,
    .writev       = pwr_pgdev_writev,
    .time         = pwr_pgdev_time,
    .clear        = pwr_pgdev_clear,
	.stat_get     = pwr_dev_stat_get,
	.stat_start   = pwr_dev_stat_start,
	.stat_stop    = pwr_dev_stat_stop,
	.stat_clear   = pwr_dev_stat_clear,
    .private_data = 0x0
};

plugin_devops_t *pwr_pgdev_init( const char *initstr )
{
    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    *dev = devops;

    dev->private_data = malloc( sizeof(pwr_pgdev_t) );
    bzero( dev->private_data, sizeof(pwr_pgdev_t) );

    if( pgdev_verbose )
        printf( "Info: initializing PWR PowerG device\n" );

    IntelEnergyLibInitialize();
    StartLog("/tmp/PowerGadgetLog.csv");

    PWR_PGDEV(dev->private_data)->num_msrs = 0;
    GetNumMsrs( &(PWR_PGDEV(dev->private_data)->num_msrs) );

    return dev;
}

int pwr_pgdev_final( plugin_devops_t *dev )
{
    if( pgdev_verbose )
        printf( "Info: finalizing PWR PowerGadget device\n" );

    StopLog();
                                                
    free( dev->private_data );
    free( dev );

    return 0;
}

pwr_fd_t pwr_pgdev_open( plugin_devops_t *dev, const char *openstr )
{
    pwr_fd_t *fd = malloc( sizeof(pwr_pgfd_t) );
    bzero( fd, sizeof(pwr_pgfd_t) );

    if( pgdev_verbose )
        printf( "Info: opening PWR PowerGadget descriptor\n" );

    PWR_PGFD(fd)->dev = PWR_PGDEV(dev->private_data);

    return fd;
}

int pwr_pgdev_close( pwr_fd_t fd )
{
    if( pgdev_verbose )
        printf( "Info: closing PWR PowerGadget device\n" );

    PWR_PGFD(fd)->dev = 0x0;
    free( fd );

    return 0;
}

int pwr_pgdev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    int func;
    int i, n;
    double data[3];
    struct timeval tv;
    
    if( pgdev_verbose )
        printf( "Info: reading from PWR PowerGadget device\n" );

    ReadSample();
    gettimeofday( &tv, NULL );

    switch( attr ) {
        case PWR_ATTR_FREQ:
            for( i = 0; i < (PWR_PGFD(fd)->dev)->num_msrs; i++ ) {
                GetMsrFunc( i, &func );
                if( func == MSR_FUNC_FREQ ) {
                    GetPowerData( 0, i, data, &n );
                    *((double *)value) = data[0];
                    break;
                }
            }
            break;
        case PWR_ATTR_POWER:
            for( i = 0; i < (PWR_PGFD(fd)->dev)->num_msrs; i++ ) {
                GetMsrFunc( i, &func );
                if( func == MSR_FUNC_POWER ) {
                    GetPowerData( 0, i, data, &n );
                    *((double *)value) = data[0];
                    break;
                }
            }
            break;
        case PWR_ATTR_ENERGY:
            for( i = 0; i < (PWR_PGFD(fd)->dev)->num_msrs; i++ ) {
                GetMsrFunc( i, &func );
                if( func == MSR_FUNC_POWER ) {
                    GetPowerData( 0, i, data, &n );
                    *((double *)value) = data[1];
                    break;
                }
            }
            break;
        case PWR_ATTR_TEMP:
            for( i = 0; i < (PWR_PGFD(fd)->dev)->num_msrs; i++ ) {
                GetMsrFunc( i, &func );
                if( func == MSR_FUNC_TEMP ) {
                    GetPowerData( 0, i, data, &n );
                    *((double *)value) = data[0];
                    break;
                }
            }
            break;
        default:
            printf( "Warning: unknown PWR reading attr (%u) requested\n", attr );
            break;
    }
    *timestamp = tv.tv_sec*1000000000ULL + tv.tv_usec*1000;

    if( pgdev_verbose )
        printf( "Info: reading of type %u at time %llu with value %lf\n",
                attr, *(unsigned long long *)timestamp, *(double *)value );

    return 0;
}

int pwr_pgdev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    if( pgdev_verbose )
        printf( "Info: writing to PWR PowerGadget device\n" );

    return 0;
}

int pwr_pgdev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    if( pgdev_verbose )
        printf( "Info: reading from PWR PowerGadget device\n" );

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_pgdev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );

    return 0;
}

int pwr_pgdev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    if( pgdev_verbose )
        printf( "Info: writing to PWR PowerGadget device\n" );

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_pgdev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

    return 0;
}

int pwr_pgdev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;

    if( pgdev_verbose )
        printf( "Info: reading time from PWR PowerGadget device\n" );

    return pwr_pgdev_read( fd, PWR_ATTR_POWER, &value, sizeof(double), timestamp );;
}

int pwr_pgdev_clear( pwr_fd_t fd )
{
    return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_pgdev_init,
    .final  = pwr_pgdev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}
