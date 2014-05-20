#include "mchw_xtpmdev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int mchw_xtpmdev_open( pwr_dev_t *dev, char *initstr )
{
    return 0;
}

int mchw_xtpmdev_close( pwr_dev_t *dev )
{
    return 0;
}

int mchw_xtpmdev_read( pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float reading[], unsigned long long *timestamp )
{
    return 0;
}

int mchw_xtpmdev_write( pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float setting[], unsigned long long *timestamp )
{
    return 0;
}

int mchw_xtpmdev_time( pwr_dev_t dev, unsigned long long *time )
{
    return 0;
}

