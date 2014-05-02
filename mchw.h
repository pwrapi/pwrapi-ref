#ifndef MCHW_H 
#define MCHW_H 

#ifdef __cplusplus
extern "C" {
#endif

typedef void* mchw_dev_t;

typedef enum {
	MCHW_VOLTS,
	MCHW_AMPS,
	MCHW_WATTS
} mchw_read_type_t;

typedef int (*mchw_init_t)(mchw_dev_t *dev);
typedef int (*mchw_final_t)(mchw_dev_t *dev);
typedef int (*mchw_open_t)(mchw_dev_t dev);
typedef int (*mchw_close_t)(mchw_dev_t dev);
typedef int (*mchw_read_t)(mchw_dev_t dev, unsigned int arraysize,
	mchw_read_type_t type[], float reading[], unsigned long long *timestamp);
typedef int (*mchw_time_t)(mchw_dev_t dev, unsigned long long *time);

typedef struct {
	mchw_init_t  init;
	mchw_final_t final;
	mchw_open_t  open;
	mchw_close_t close;
	mchw_read_t  read;
	mchw_time_t  time;
} mchw_t;

#ifdef __cplusplus
}
#endif

#endif
