/*
Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#ifndef PWR_MSRDEV_H 
#define PWR_MSRDEV_H 

#include "pwrdev.h"

#ifdef __cplusplus
extern "C" {
#endif

plugin_devops_t *pwr_msrdev_init( const char *initstr );
int pwr_msrdev_final( plugin_devops_t *dev );

pwr_fd_t pwr_msrdev_open( plugin_devops_t *dev, const char *openstr );
int pwr_msrdev_close( pwr_fd_t fd );

int pwr_msrdev_read( pwr_fd_t fd, PWR_AttrName attr,
    void *value, unsigned int len, PWR_Time *timestamp );
int pwr_msrdev_write( pwr_fd_t fd, PWR_AttrName attr,
    void *value, unsigned int len );

int pwr_msrdev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] );
int pwr_msrdev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] );

int pwr_msrdev_time( pwr_fd_t fd, PWR_Time *timestamp );
int pwr_msrdev_clear( pwr_fd_t fd );

#ifdef __cplusplus
}
#endif

#endif
