#ifndef MCHW_XTPMDEV_H 
#define MCHW_XTPMDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

pwr_dev_t mchw_xtpmdev_init( const char *initstr );
int mchw_xtpmdev_final( pwr_dev_t dev );

pwr_fd_t mchw_xtpmdev_open( pwr_dev_t dev, const char *openstr );
int mchw_xtpmdev_close( pwr_fd_t fd );

int mchw_xtpmdev_read( pwr_fd_t fd, PWR_AttrName attr,
    void *value, unsigned int len, PWR_Time *timestamp );
int mchw_xtpmdev_write( pwr_fd_t fd, PWR_AttrName attr,
    void *value, unsigned int len );

int mchw_xtpmdev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] );
int mchw_xtpmdev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] );

int mchw_xtpmdev_time( pwr_fd_t fd, PWR_Time *timestamp );
int mchw_xtpmdev_clear( pwr_fd_t fd );

#ifdef __cplusplus
}
#endif

#endif
