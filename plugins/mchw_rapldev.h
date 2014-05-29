#ifndef MCHW_RAPLDEV_H 
#define MCHW_RAPLDEV_H 

#include "dev.h"

#ifdef __cplusplus
extern "C" {
#endif

pwr_dev_t mchw_rapldev_open(const char *initstr);
int mchw_rapldev_close(pwr_dev_t dev);
int mchw_rapldev_read(pwr_dev_t dev, unsigned int arraysize, PWR_Value value[] );
int mchw_rapldev_write(pwr_dev_t dev, unsigned int arraysize, PWR_Value value[] );
int mchw_rapldev_time(pwr_dev_t dev, unsigned long long *time);

#ifdef __cplusplus
}
#endif

#endif
