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

#ifndef _PWR_DEBUG_H
#define _PWR_DEBUG_H

#include <sys/types.h>
#include <unistd.h>
#ifndef NO_CONFIG 
#include <pwr_config.h>
#endif
extern unsigned int _DbgFlags;

#define DBG_BASE (1<<0)
#define DBG_CONFIG (1<<1)
#define DBG_EC (1<<2)
#define DBG_EC2 (1<<3)
#define DBG_PLUGGIN (1<<4)

#ifdef USE_DEBUG

#ifndef gettid
#include <sys/syscall.h>
// equivalent to:  pid_t  gettid(void)
#define getid() (pid_t)syscall(SYS_gettid)
#endif

#include <stdlib.h>
#include <stdio.h>

#define DBGX( fmt, ... ) DBGX2( 0x1, fmt, ## __VA_ARGS__ )

#ifdef __cplusplus
#include <cxxabi.h>
#define DBGX2( flag, fmt, ... ) \
{\
    if ( flag & _DbgFlags ) {\
        char* realname = abi::__cxa_demangle(typeid(*this).name(),0,0,NULL);\
        fprintf( stderr, "%d:%s::%s():%d: "fmt, getid(), realname ? realname : "?????????", \
                        __func__, __LINE__, ## __VA_ARGS__ );\
        fflush(stderr);\
        if ( realname ) free(realname);\
    }\
}
#endif

#define DBG( fmt, ... ) DBG2( 0x1, fmt, ## __VA_ARGS__ )

#define DBG2( flag, fmt, ... ) DBG3( flag, "", fmt, ## __VA_ARGS__ ) 
#define DBG4( pre, fmt, ... ) DBG3( 0x1, pre, fmt, ## __VA_ARGS__ ) 

#define DBG3( flag, prefix, fmt, ... ) \
    if ( flag & _DbgFlags ) {\
    	fprintf( stderr, "%d:%s:%s():%d: "fmt, getid(), prefix, __func__, __LINE__, ##__VA_ARGS__);\
	}

#else

#define DBG( fmt, ... )
#define DBGX( fmt, ... )
#define DBGX2( flag, fmt, ... )
#define DBG2( fmt, ... )
#define DBG3( flag, prefix, fmt, ... )
#define DBG4( pre, fmt, ... )

#endif

#endif
