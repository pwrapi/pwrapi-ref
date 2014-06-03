#ifndef MCHW_XTPMDEV_H 
#define MCHW_XTPMDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

pwr_dev_t mchw_xtpmdev_open( const char *initstr );
int mchw_xtpmdev_close( pwr_dev_t dev );

int mchw_xtpmdev_read( pwr_dev_t dev, PWR_AttrType type,
    void *value, unsigned int len, unsigned long long *time );
int mchw_xtpmdev_write( pwr_dev_t dev, PWR_AttrType type,
    void *value, unsigned int len );

int mchw_xtpmdev_readv( pwr_dev_t dev, unsigned int arraysize,
    PWR_Value value[], int status[] );
int mchw_xtpmdev_writev( pwr_dev_t dev, unsigned int arraysize,
    PWR_Value value[], int status[] );

int mchw_xtpmdev_time( pwr_dev_t dev, unsigned long long *time );
int mchw_xtpmdev_clear( pwr_dev_t dev );

#ifdef __cplusplus
}
#endif

#endif
