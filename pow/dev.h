/* 
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 */

#ifndef _PWR_DEV_H
#define _PWR_DEV_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* pwr_fd_t;

struct plugin_devops_t;

typedef pwr_fd_t (*pwr_open_t)( struct plugin_devops_t*, const char *openstr );
typedef int (*pwr_close_t)( pwr_fd_t fd );

typedef int (*pwr_read_t)( pwr_fd_t fd, PWR_AttrName name,
    void *value, unsigned int len, PWR_Time *time );
typedef int (*pwr_write_t)( pwr_fd_t dev, PWR_AttrName name,
    void *value, unsigned int len );

typedef int (*pwr_readv_t)( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName names[], void* ptr, PWR_Time ts[], int status[] );
typedef int (*pwr_writev_t)( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName names[], void* ptr, int status[] );

typedef int (*pwr_time_t)( pwr_fd_t fd, PWR_Time *time );
typedef int (*pwr_clear_t)( pwr_fd_t fd );

typedef int (*statOp_t)( int num, double input[], double* result, PWR_Time* ts);

typedef int (*pwr_stat_start_t)( pwr_fd_t fd, PWR_AttrName name );
typedef int (*pwr_stat_stop_t)( pwr_fd_t fd, PWR_AttrName name );
typedef int (*pwr_stat_clear_t)( pwr_fd_t fd, PWR_AttrName name );
typedef int (*pwr_stat_get_t)( pwr_fd_t fd, PWR_AttrName name, statOp_t,
									void* result, PWR_StatTimes* ts );

typedef struct plugin_devops_t {
    pwr_open_t  open;
    pwr_close_t close;

    pwr_read_t  read;
    pwr_write_t write;

    pwr_readv_t  readv;
    pwr_writev_t writev;

    pwr_time_t  time;
    pwr_clear_t clear;

	pwr_stat_start_t stat_start;
	pwr_stat_stop_t  stat_stop;
	pwr_stat_clear_t stat_clear;
	pwr_stat_get_t stat_get;

    void *private_data;

} plugin_devops_t;

typedef plugin_devops_t* (*pwr_init_t)( const char *initstr );
typedef int (*pwr_final_t)( plugin_devops_t* );

typedef struct {
    pwr_init_t  init;
    pwr_final_t final;
} plugin_dev_t;

#define GETDEVFUNC "getDev"
typedef plugin_dev_t* (*getDevFuncPtr_t)(void); 

#ifdef __cplusplus
}
#endif

#endif
