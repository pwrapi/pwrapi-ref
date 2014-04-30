#ifndef MCHW_PIDEV_H 
#define MCHW_PIDEV_H 

#include "mchw.h"

#ifdef __cplusplus
extern "C" {
#endif

void mchw_pidev_init_t(mchw_dev_t *dev);
void mchw_pidev_final_t(mchw_dev_t *dev);
void mchw_pidev_open_t(mchw_dev_t *dev);
void mchw_pidev_close_t(mchw_dev_t *dev);
void mchw_pidev_read_t(mchw_dev_t *dev, mchw_reading_t *reading);
void mchw_pidev_time_t(mchw_dev_t *dev, mchw_time_t *time);

#ifdef __cplusplus
}
#endif

#endif
