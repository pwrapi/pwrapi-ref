#ifndef MCHW_H 
#define MCHW_H 

#ifdef __cplusplus
extern "C" {
#endif

typedef void* mchw_dev_t;

typedef struct {
        float volts;
        float amps;
        float watts;
} mchw_reading_t;

typedef unsigned long long mchw_time_t;

typedef (*mchw_init_t)(mchw_dev_t *dev);
typedef (*mchw_final_t)(mchw_dev_t *dev);
typedef (*mchw_open_t)(mchw_dev_t *dev);
typedef (*mchw_close_t)(mchw_dev_t *dev);
typedef (*mchw_read_t)(mchw_dev_t *dev, mchw_reading_t *reading);
typedef (*mchw_time_t)(mchw_dev_t *dev, mchw_time_t *time);

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
