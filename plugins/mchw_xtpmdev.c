#include "mchw_xtpmdev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

pwr_dev_t mchw_xtpmdev_init( const char *initstr )
{
    return 0x0;
}

int mchw_xtpmdev_final( pwr_dev_t dev )
{
    return 0;
}

pwr_fd_t mchw_xtpmdev_open( pwr_dev_t dev, const char *openstr )
{
    return 0x0;
}

int mchw_xtpmdev_close( pwr_fd_t fd )
{
    return 0;
}

int mchw_xtpmdev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    return 0;
}

int mchw_xtpmdev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    return 0;
}

int mchw_xtpmdev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    return 0;
}

int mchw_xtpmdev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    return 0;
}

int mchw_xtpmdev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    return 0;
}

int mchw_xtpmdev_clear( pwr_fd_t fd )
{
    return 0;
}

static plugin_dev_t dev = {
    .init   = mchw_xtpmdev_init,
    .final  = mchw_xtpmdev_final,
    .open   = mchw_xtpmdev_open,
    .close  = mchw_xtpmdev_close,
    .read   = mchw_xtpmdev_read,
    .write  = mchw_xtpmdev_write,
    .readv  = mchw_xtpmdev_readv,
    .writev = mchw_xtpmdev_writev,
    .time   = mchw_xtpmdev_time,
    .clear  = mchw_xtpmdev_clear
};

plugin_dev_t* getDev() {
    return &dev;
}
