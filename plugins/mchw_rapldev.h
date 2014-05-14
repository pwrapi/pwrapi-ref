#ifndef MCHW_RAPLDEV_H 
#define MCHW_RAPLDEV_H 

#include "mchw.h"

#ifdef __cplusplus
extern "C" {
#endif

int mchw_rapldev_init(mchw_dev_t *dev, char *initstr);
int mchw_rapldev_final(mchw_dev_t *dev);
int mchw_rapldev_read(mchw_dev_t dev, unsigned int arraysize,
	mchw_read_type_t type[], float reading[], unsigned long long *timestamp);
int mchw_rapldev_time(mchw_dev_t dev, unsigned long long *time);

#ifdef __cplusplus
}
#endif

#endif
