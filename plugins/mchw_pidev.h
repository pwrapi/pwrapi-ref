#ifndef MCHW_PIDEV_H 
#define MCHW_PIDEV_H 

#include "mchw.h"

#ifdef __cplusplus
extern "C" {
#endif

void mchw_pidev_init(mchw_dev_t *dev);
void mchw_pidev_final(mchw_dev_t *dev);
void mchw_pidev_open(mchw_dev_t *dev);
void mchw_pidev_close(mchw_dev_t *dev);
void mchw_pidev_read(mchw_dev_t *dev, mchw_reading_t *reading);
void mchw_pidev_time(mchw_dev_t *dev, mchw_time_t *time);

#ifdef __cplusplus
}
#endif

#endif
