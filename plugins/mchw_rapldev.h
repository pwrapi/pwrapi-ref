#ifndef MCHW_RAPLDEV_H 
#define MCHW_RAPLDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

int mchw_rapldev_open(pwr_dev_t *dev, char *initstr);
int mchw_rapldev_close(pwr_dev_t *dev);
int mchw_rapldev_read(pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float reading[], unsigned long long *timestamp);
int mchw_rapldev_write(pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float setting[], unsigned long long *timestamp);
int mchw_rapldev_time(pwr_dev_t dev, unsigned long long *time);

#ifdef __cplusplus
}
#endif

#endif
