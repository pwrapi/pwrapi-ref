#ifndef MCHW_H 
#define MCHW_H 

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* mchw_dev_t;

typedef int (*mchw_open_t)(mchw_dev_t *dev, char *initstr);
typedef int (*mchw_close_t)(mchw_dev_t *dev);
typedef int (*mchw_read_t)(mchw_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float reading[], unsigned long long *timestamp);
typedef int (*mchw_write_t)(mchw_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float setting[], unsigned long long *timestamp);
typedef int (*mchw_time_t)(mchw_dev_t dev, unsigned long long *time);

typedef struct {
    mchw_open_t  open;
    mchw_close_t close;
    mchw_read_t  read;
    mchw_write_t write;
    mchw_time_t  time;
} mchw_t;

#ifdef __cplusplus
}
#endif

#endif
