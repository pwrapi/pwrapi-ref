#ifndef PWR_DEV_H 
#define PWR_DEV_H 

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* pwr_dev_t;

typedef int (*pwr_open_t)(pwr_dev_t *dev, char *initstr);
typedef int (*pwr_close_t)(pwr_dev_t *dev);
typedef int (*pwr_read_t)(pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float reading[], unsigned long long *timestamp);
typedef int (*pwr_write_t)(pwr_dev_t dev, unsigned int arraysize,
    PWR_AttrType type[], float setting[], unsigned long long *timestamp);
typedef int (*pwr_time_t)(pwr_dev_t dev, unsigned long long *time);

typedef struct {
    pwr_open_t  open;
    pwr_close_t close;
    pwr_read_t  read;
    pwr_write_t write;
    pwr_time_t  time;
} plugin_dev_t;

#ifdef __cplusplus
}
#endif

#endif
