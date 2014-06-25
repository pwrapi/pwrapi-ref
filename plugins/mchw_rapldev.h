#ifndef MCHW_RAPLDEV_H 
#define MCHW_RAPLDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

pwr_dev_t mchw_rapldev_init( const char *initstr );
int mchw_rapldev_final( pwr_dev_t dev );

pwr_fd_t mchw_rapldev_open( pwr_dev_t dev, const char *openstr );
int mchw_rapldev_close( pwr_fd_t fd );

int mchw_rapldev_read( pwr_fd_t fd, PWR_AttrName attr,
    void *value, unsigned int len, PWR_Time *timestamp );
int mchw_rapldev_write( pwr_fd_t fd, PWR_AttrName attr,
    void *value, unsigned int len );

int mchw_rapldev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] );
int mchw_rapldev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] );

int mchw_rapldev_time( pwr_fd_t fd, PWR_Time *timestamp );
int mchw_rapldev_clear( pwr_fd_t fd );

#ifdef __cplusplus
}
#endif

#endif
