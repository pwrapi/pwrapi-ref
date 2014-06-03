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

int mchw_xtpmdev_read( pwr_dev_t dev, PWR_AttrType type, void *value, unsigned int len, unsigned long long *time )
{
    return 0;
}

int mchw_xtpmdev_write( pwr_dev_t dev, PWR_AttrType type, void *value, unsigned int len )
{
    return 0;
}

int mchw_xtpmdev_readv( pwr_dev_t dev, unsigned int arraysize, PWR_Value value[], int status[] )
{
    return 0;
}

int mchw_xtpmdev_writev( pwr_dev_t dev, unsigned int arraysize, PWR_Value value[], int status[] )
{
    return 0;
}

int mchw_xtpmdev_time( pwr_dev_t dev, unsigned long long *time )
{
    return 0;
}

int mchw_xtpmdev_clear( pwr_dev_t dev )
{
    return 0;
}

