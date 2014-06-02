#ifndef PWR_DEV_H 
#define PWR_DEV_H 

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* pwr_dev_t;

typedef pwr_dev_t (*pwr_open_t)( const char *initstr );
typedef int (*pwr_close_t)( pwr_dev_t dev );

typedef int (*pwr_readv_t)( pwr_dev_t dev, unsigned int arraysize,
                                        PWR_Value values[], int status[] );
typedef int (*pwr_writev_t)( pwr_dev_t dev, unsigned int arraysize,
                                        PWR_Value values[], int status[] );

typedef int (*pwr_read_t)( pwr_dev_t dev, PWR_AttrType, void*, size_t,
                                                        PWR_Time*  );
typedef int (*pwr_write_t)( pwr_dev_t dev, PWR_AttrType, void*, size_t );

typedef int (*pwr_time_t)( pwr_dev_t dev, unsigned long long *time );

typedef struct {
    pwr_open_t  open;
    pwr_close_t close;
    pwr_read_t  read;
    pwr_write_t write;
    pwr_readv_t  readv;
    pwr_writev_t writev;
    pwr_time_t  time;
} plugin_dev_t;

#ifdef __cplusplus
}
#endif

#endif
