#ifndef MCHW_PIDEV_H 
#define MCHW_PIDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

int mchw_pidev_open(pwr_dev_t *dev, char *initstr);
int mchw_pidev_close(pwr_dev_t *dev);
int mchw_pidev_read(pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float reading[], unsigned long long *timestamp);
int mchw_pidev_write(pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float setting[], unsigned long long *timestamp);
int mchw_pidev_time(pwr_dev_t dev, unsigned long long *time);

#ifdef __cplusplus
}
#endif

#endif
