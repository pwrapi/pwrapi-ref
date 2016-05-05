/* 
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#ifndef _PWR_DEV_H
#define _PWR_DEV_H

#include "pwrtypes.h"

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

typedef int (*pwr_get_samples_t)( pwr_fd_t fd, PWR_AttrName name, 
			PWR_Time* time, double period, unsigned int*, void* );

typedef int (*pwr_log_start_t)( pwr_fd_t fd, PWR_AttrName name );
typedef int (*pwr_log_stop_t)( pwr_fd_t fd, PWR_AttrName name );

typedef struct plugin_devops_t {
    pwr_open_t  open;
    pwr_close_t close;

    pwr_read_t  read;
    pwr_write_t write;

    pwr_readv_t  readv;
    pwr_writev_t writev;

    pwr_time_t  time;
    pwr_clear_t clear;

	pwr_log_start_t log_start;
	pwr_log_stop_t  log_stop;
	pwr_get_samples_t  get_samples;

    void *private_data;

} plugin_devops_t;

typedef plugin_devops_t* (*pwr_init_t)( const char *initstr );
typedef int (*pwr_final_t)( plugin_devops_t* );

typedef struct {
    pwr_init_t  init;
    pwr_final_t final;

} plugin_dev_t;


typedef int (*pwr_dev_num_objs)(void);
typedef int (*pwr_dev_read_objs)(int,PWR_ObjType*);
typedef int (*pwr_dev_num_attrs)(PWR_ObjType);
typedef int (*pwr_dev_read_attrs)(int,PWR_AttrName*);
typedef int (*pwr_dev_get_dev_name)(PWR_ObjType type, size_t, char* );
typedef int (*pwr_dev_get_dev_open_str)(PWR_ObjType type,
                                int global_index, size_t, char* );
typedef int (*pwr_dev_get_dev_init_str)(const char*, size_t, char* );
typedef int (*pwr_dev_get_plugin_name)( size_t, char* );

typedef struct {
	pwr_dev_num_objs numObjs;
	pwr_dev_read_objs readObjs;
	pwr_dev_num_attrs numAttrs;
	pwr_dev_read_attrs readAttrs;
    pwr_dev_get_dev_name getDevName;
    pwr_dev_get_dev_open_str getDevOpenStr;
    pwr_dev_get_dev_init_str getDevInitStr;
    pwr_dev_get_plugin_name getPluginName;
} plugin_meta_t;

#define GETDEVFUNC "getDev"
typedef plugin_dev_t* (*getDevFuncPtr_t)(void); 

#define GETMETAFUNC "getMeta"
typedef plugin_meta_t* (*getMetaFuncPtr_t)(void);

#ifdef __cplusplus
}
#endif

#endif
