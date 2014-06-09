#include "mchw_xtpmdev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

pwr_dev_t mchw_xtpmdev_open( const char *initstr )
{
    return 0x0;
}

int mchw_xtpmdev_close( pwr_dev_t dev )
{
    return 0;
}

int mchw_xtpmdev_read( pwr_dev_t dev, PWR_AttrType type, void *value, unsigned int len, unsigned long long *timestamp )
{
    return 0;
}

int mchw_xtpmdev_write( pwr_dev_t dev, PWR_AttrType type, void *value, unsigned int len )
{
    return 0;
}

int mchw_xtpmdev_readv( pwr_dev_t dev, unsigned int arraysize,
    const PWR_AttrType types[], void *values, unsigned long long timestamp[], int status[] )
{
    return 0;
}

int mchw_xtpmdev_writev( pwr_dev_t dev, unsigned int arraysize,
    const PWR_AttrType types[], void *values, int status[] )
{
    return 0;
}

int mchw_xtpmdev_time( pwr_dev_t dev, unsigned long long *timestamp )
{
    return 0;
}

int mchw_xtpmdev_clear( pwr_dev_t dev )
{
    return 0;
}

static plugin_dev_t dev = {
    open   : mchw_xtpmdev_open,
    close  : mchw_xtpmdev_close,
    read   : mchw_xtpmdev_read,
    write  : mchw_xtpmdev_write,
    readv  : mchw_xtpmdev_readv,
    writev : mchw_xtpmdev_writev,
    time   : mchw_xtpmdev_time,
    clear  : mchw_xtpmdev_clear
};

plugin_dev_t* getDev() {
    return &dev;
}
