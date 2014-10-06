#ifndef OSHW_CPUDEV_H 
#define OSHW_CPUDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

plugin_devops_t *oshw_cpudev_init( const char *initstr );
int oshw_cpudev_final( plugin_devops_t *dev );

pwr_fd_t oshw_cpudev_open( plugin_devops_t *dev, const char *openstr );
int oshw_cpudev_close( pwr_fd_t fd );

int oshw_cpudev_read( pwr_fd_t fd, PWR_AttrName attr,
    void *value, unsigned int len, PWR_Time *timestamp );
int oshw_cpudev_write( pwr_fd_t fd, PWR_AttrName attr,
    void *value, unsigned int len );

int oshw_cpudev_readv(pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] );
int oshw_cpudev_writev(pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] );

int oshw_cpudev_time( pwr_fd_t fd, PWR_Time *timestamp );
int oshw_cpudev_clear( pwr_fd_t fd );

#ifdef __cplusplus
}
#endif

#endif
