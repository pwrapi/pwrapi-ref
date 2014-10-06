#include "mchw_xtpmdev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int xtpmdev_verbose = 0;

typedef struct {
    int fd;
} mchw_xtpmdev_t;
#define MCHW_XTPMDEV(X) ((mchw_xtpmdev_t *)(X))

typedef struct {
    mchw_xtpmdev_t *dev;
} mchw_wufd_t;
#define MCHW_XTPMFD(X) ((mchw_xtpmfd_t *)(X))

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

plugin_devops_t *mchw_xtpmdev_init( const char *initstr )
{
    if( xtpmdev_verbose )
        printf( "Info: initializing MCHW XTPM device\n" );

    return 0x0;
}

int mchw_xtpmdev_final( plugin_devops_t *dev )
{
    if( xtpmdev_verbose )
        printf( "Info: finalizing MCHW XTPM device\n" );

    return 0;
}

pwr_fd_t mchw_xtpmdev_open( plugin_devops_t *dev, const char *openstr )
{
    if( xtpmdev_verbose )
        printf( "Info: opening MCHW XTPM device\n" );

    return 0x0;
}

int mchw_xtpmdev_close( pwr_fd_t fd )
{
    if( xtpmdev_verbose )
        printf( "Info: closing MCHW XTPM device\n" );

    return 0;
}

int mchw_xtpmdev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    if( xtpmdev_verbose )
        printf( "Info: reading from MCHW XTPM device\n" );

    return 0;
}

int mchw_xtpmdev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    if( xtpmdev_verbose )
        printf( "Info: writing to MCHW XTPM device\n" );

    return 0;
}

int mchw_xtpmdev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    if( xtpmdev_verbose )
        printf( "Info: reading from MCHW XTPM device\n" );

    return 0;
}

int mchw_xtpmdev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    if( xtpmdev_verbose )
        printf( "Info: writing to MCHW XTPM device\n" );

    return 0;
}

int mchw_xtpmdev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    if( xtpmdev_verbose )
        printf( "Info: reading time from MCHW XTPM device\n" );

    return 0;
}

int mchw_xtpmdev_clear( pwr_fd_t fd )
{
    return 0;
}

static plugin_devops_t devops = {
    .open   = mchw_xtpmdev_open,
    .close  = mchw_xtpmdev_close,
    .read   = mchw_xtpmdev_read,
    .write  = mchw_xtpmdev_write,
    .readv  = mchw_xtpmdev_readv,
    .writev = mchw_xtpmdev_writev,
    .time   = mchw_xtpmdev_time,
    .clear  = mchw_xtpmdev_clear
};

static plugin_dev_t dev = {
    .init   = mchw_xtpmdev_init,
    .final  = mchw_xtpmdev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}
