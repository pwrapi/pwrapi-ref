#ifndef PWR_DEV_H 
#define PWR_DEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

int pwr_dev_stat_start( pwr_fd_t fd, PWR_AttrName name );
int pwr_dev_stat_stop( pwr_fd_t fd, PWR_AttrName name );

int pwr_dev_stat_clear( pwr_fd_t fd, PWR_AttrName name );

int pwr_dev_stat_get( pwr_fd_t fd, PWR_AttrName name, statOp_t op,
    void* result, PWR_StatTimes* ts );

#ifdef __cplusplus
}
#endif

#endif
