#ifndef MCHW_PIDEV_H 
#define MCHW_PIDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

pwr_dev_t mchw_pidev_open( const char *initstr );
int mchw_pidev_close( pwr_dev_t dev );

int mchw_pidev_read( pwr_dev_t dev, PWR_AttrType type,
    void *value, unsigned int len, unsigned long long *timestamp );
int mchw_pidev_write( pwr_dev_t dev, PWR_AttrType type,
    void *value, unsigned int len );

int mchw_pidev_readv( pwr_dev_t dev, unsigned int arraysize,
    PWR_Value value[], int status[] );
int mchw_pidev_writev( pwr_dev_t dev, unsigned int arraysize,
    PWR_Value value[], int status[] );

int mchw_pidev_time( pwr_dev_t dev, unsigned long long *timestamp );
int mchw_pidev_clear( pwr_dev_t dev );

#ifdef __cplusplus
}
#endif

#endif
