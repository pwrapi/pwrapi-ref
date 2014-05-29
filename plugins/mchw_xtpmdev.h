#ifndef MCHW_XTPMDEV_H 
#define MCHW_XTPMDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

pwr_dev_t mchw_xtpmdev_open(const char *initstr);
int mchw_xtpmdev_close(pwr_dev_t dev);
int mchw_xtpmdev_read(pwr_dev_t dev, unsigned int arraysize, PWR_Value value[] );
int mchw_xtpmdev_write(pwr_dev_t dev, unsigned int arraysize, PWR_Value value[] );
int mchw_xtpmdev_time(pwr_dev_t dev, unsigned long long *time);

#ifdef __cplusplus
}
#endif

#endif
