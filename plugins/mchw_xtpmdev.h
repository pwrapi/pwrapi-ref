#ifndef MCHW_XTPMDEV_H 
#define MCHW_XTPMDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

pwr_dev_t mchw_xtpmdev_open( const char *initstr );
int mchw_xtpmdev_close( pwr_dev_t dev );

int mchw_xtpmdev_read( pwr_dev_t dev, PWR_AttrName attr,
    void *value, unsigned int len, PWR_Time *timestamp );
int mchw_xtpmdev_write( pwr_dev_t dev, PWR_AttrName attr,
    void *value, unsigned int len );

int mchw_xtpmdev_readv( pwr_dev_t dev, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] );
int mchw_xtpmdev_writev( pwr_dev_t dev, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] );

int mchw_xtpmdev_time( pwr_dev_t dev, PWR_Time *timestamp );
int mchw_xtpmdev_clear( pwr_dev_t dev );

#ifdef __cplusplus
}
#endif

#endif
