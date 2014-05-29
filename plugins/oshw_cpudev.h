#ifndef OSHW_CPUDEV_H 
#define OSHW_CPUDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

int oshw_cpudev_open(pwr_dev_t *dev, char *initstr);
int oshw_cpudev_close(pwr_dev_t *dev);
int oshw_cpudev_read(pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float reading[], unsigned long long *timestamp);
int oshw_cpudev_write(pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float setting[], unsigned long long *timestamp);
int oshw_cpudev_time(pwr_dev_t dev, unsigned long long *time);

#ifdef __cplusplus
}
#endif

#endif
