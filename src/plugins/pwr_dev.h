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

#ifndef PWR_DEV_H 
#define PWR_DEV_H 

#include "pwrdev.h"
#include "pwr_config.h"
#include "debug.h"

#define DBGP(X, ... ) DBG3( DBG_PLUGGIN, "Plugin", X, ##__VA_ARGS__ )

#ifdef __cplusplus
extern "C" {
#endif

int pwr_dev_log_start( pwr_fd_t fd, PWR_AttrName name );
int pwr_dev_log_stop( pwr_fd_t fd, PWR_AttrName name );

#ifdef __cplusplus
}
#endif

#endif
