#ifndef MCHW_RAPLDEV_H 
#define MCHW_RAPLDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

pwr_dev_t mchw_rapldev_open( const char *initstr );
int mchw_rapldev_close( pwr_dev_t dev );

int mchw_rapldev_read( pwr_dev_t dev, PWR_AttrType type,
    void *value, unsigned int len, unsigned long long *timestamp );
int mchw_rapldev_write( pwr_dev_t dev, PWR_AttrType type,
    void *value, unsigned int len );

int mchw_rapldev_readv( pwr_dev_t dev, unsigned int arraysize,
    const PWR_AttrType types[], void *values, unsigned long long timestamp[], int status[] );
int mchw_rapldev_writev( pwr_dev_t dev, unsigned int arraysize,
    const PWR_AttrType types[], void *values, int status[] );

int mchw_rapldev_time( pwr_dev_t dev, unsigned long long *timestamp );
int mchw_rapldev_clear( pwr_dev_t dev );

#ifdef __cplusplus
}
#endif

#endif
