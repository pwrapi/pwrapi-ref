#ifndef OSHW_CPUDEV_H 
#define OSHW_CPUDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

pwr_dev_t oshw_cpudev_open( const char *initstr );
int oshw_cpudev_close( pwr_dev_t dev );

int oshw_cpudev_read( pwr_dev_t dev, PWR_AttrType type,
    void *value, unsigned int len, PWR_Time *timestamp );
int oshw_cpudev_write( pwr_dev_t dev, PWR_AttrType type,
    void *value, unsigned int len );

int oshw_cpudev_readv(pwr_dev_t dev, unsigned int arraysize,
    const PWR_AttrType types[], void *values, PWR_Time timestamp[], int status[] );
int oshw_cpudev_writev(pwr_dev_t dev, unsigned int arraysize,
    const PWR_AttrType types[], void *values, int status[] );

int oshw_cpudev_time( pwr_dev_t dev, PWR_Time *timestamp );
int oshw_cpudev_clear( pwr_dev_t dev );

#ifdef __cplusplus
}
#endif

#endif
