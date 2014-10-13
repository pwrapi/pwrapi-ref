#ifndef PWR_XTPMDEV_H 
#define PWR_XTPMDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

plugin_devops_t *pwr_wudev_init( const char *initstr );
int pwr_wudev_final( plugin_devops_t *dev );

pwr_fd_t pwr_wudev_open( plugin_devops_t *dev, const char *openstr );
int pwr_wudev_close( pwr_fd_t fd );

int pwr_wudev_read( pwr_fd_t fd, PWR_AttrName attr,
    void *value, unsigned int len, PWR_Time *timestamp );
int pwr_wudev_write( pwr_fd_t fd, PWR_AttrName attr,
    void *value, unsigned int len );

int pwr_wudev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] );
int pwr_wudev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] );

int pwr_wudev_time( pwr_fd_t fd, PWR_Time *timestamp );
int pwr_wudev_clear( pwr_fd_t fd );

#ifdef __cplusplus
}
#endif

#endif
