#include "mchw_wudev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

pwr_dev_t mchw_wudev_init( const char *initstr )
{
    return 0x0;
}

int mchw_wudev_final( pwr_dev_t dev )
{
    return 0;
}

pwr_fd_t mchw_wudev_open( pwr_dev_t dev, const char *openstr )
{
    return 0x0;
}

int mchw_wudev_close( pwr_fd_t fd )
{
    return 0;
}

int mchw_wudev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    return 0;
}

int mchw_wudev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    return 0;
}

int mchw_wudev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    return 0;
}

int mchw_wudev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    return 0;
}

int mchw_wudev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    return 0;
}

int mchw_wudev_clear( pwr_fd_t fd )
{
    return 0;
}

static plugin_dev_t dev = {
    .init   = mchw_wudev_init,
    .final  = mchw_wudev_final,
    .open   = mchw_wudev_open,
    .close  = mchw_wudev_close,
    .read   = mchw_wudev_read,
    .write  = mchw_wudev_write,
    .readv  = mchw_wudev_readv,
    .writev = mchw_wudev_writev,
    .time   = mchw_wudev_time,
    .clear  = mchw_wudev_clear
};

plugin_dev_t* getDev() {
    return &dev;
}
