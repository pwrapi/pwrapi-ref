#ifndef MCHW_PIDEV_H 
#define MCHW_PIDEV_H 

#include "mchw.h"

#ifdef __cplusplus
extern "C" {
#endif

int mchw_pidev_init(mchw_dev_t *dev);
int mchw_pidev_final(mchw_dev_t *dev);
int mchw_pidev_open(mchw_dev_t dev);
int mchw_pidev_close(mchw_dev_t dev);
int mchw_pidev_read(mchw_dev_t dev, unsigned int arraysize,
	mchw_read_type_t type[], float reading[], mchw_time_t *timestamp);
int mchw_pidev_time(mchw_dev_t dev, mchw_time_t *time);

#ifdef __cplusplus
}
#endif

#endif
