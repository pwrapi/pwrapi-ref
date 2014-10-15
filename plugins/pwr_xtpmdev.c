#include "pwr_xtpmdev.h"
#include "pwr_dev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int xtpmdev_verbose = 0;

typedef struct {
    int fd;
} pwr_xtpmdev_t;
#define PWR_XTPMDEV(X) ((pwr_xtpmdev_t *)(X))

typedef struct {
    pwr_xtpmdev_t *dev;
} pwr_wufd_t;
#define PWR_XTPMFD(X) ((pwr_xtpmfd_t *)(X))

static plugin_devops_t devops = {
    .open         = pwr_xtpmdev_open,
    .close        = pwr_xtpmdev_close,
    .read         = pwr_xtpmdev_read,
    .write        = pwr_xtpmdev_write,
    .readv        = pwr_xtpmdev_readv,
    .writev       = pwr_xtpmdev_writev,
    .time         = pwr_xtpmdev_time,
    .clear        = pwr_xtpmdev_clear,
	.stat_get     = pwr_dev_stat_get,
	.stat_start   = pwr_dev_stat_start,
	.stat_stop    = pwr_dev_stat_stop,
	.stat_clear   = pwr_dev_stat_clear,
    .private_data = 0x0
};

static int xtpmdev_parse( const char *initstr )
{
    char *token;

    if( xtpmdev_verbose )
        printf( "Info: received initialization string %s\n", initstr );

    if( (token = strtok( (char *)initstr, ":" )) == 0x0 ) {
        printf( "Error: missing seperator in initialization string %s\n", initstr );
        return -1;
    }

    return 0;
}

static int xtpmdev_open( )
{
    return 0;
}

static int xtpmdev_read( )
{
    return 0;
}

static int xtpmdev_write( )
{
    return 0;
}

plugin_devops_t *pwr_xtpmdev_init( const char *initstr )
{
    if( xtpmdev_verbose )
        printf( "Info: initializing PWR XTPM device\n" );

    return 0x0;
}

int pwr_xtpmdev_final( plugin_devops_t *dev )
{
    if( xtpmdev_verbose )
        printf( "Info: finalizing PWR XTPM device\n" );

    return 0;
}

pwr_fd_t pwr_xtpmdev_open( plugin_devops_t *dev, const char *openstr )
{
    if( xtpmdev_verbose )
        printf( "Info: opening PWR XTPM device\n" );

    return 0x0;
}

int pwr_xtpmdev_close( pwr_fd_t fd )
{
    if( xtpmdev_verbose )
        printf( "Info: closing PWR XTPM device\n" );

    return 0;
}

int pwr_xtpmdev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    if( xtpmdev_verbose )
        printf( "Info: reading from PWR XTPM device\n" );

    return 0;
}

int pwr_xtpmdev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    if( xtpmdev_verbose )
        printf( "Info: writing to PWR XTPM device\n" );

    return 0;
}

int pwr_xtpmdev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_xtpmdev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );

    return 0;
}

int pwr_xtpmdev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_xtpmdev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

    return 0;
}

int pwr_xtpmdev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    if( xtpmdev_verbose )
        printf( "Info: reading time from PWR XTPM device\n" );

    return 0;
}

int pwr_xtpmdev_clear( pwr_fd_t fd )
{
    return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_xtpmdev_init,
    .final  = pwr_xtpmdev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}
