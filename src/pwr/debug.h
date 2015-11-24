/* 
 * Copyright 2014-2015 Sandia Corporation. Under the terms of Contract
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

#ifndef NO_CONFIG 
#include <pwr_config.h>
#endif
extern unsigned int _DbgFlags;

#define DBG_CONFIG (1<<1)
#define DBG_EC (1<<2)

#ifdef USE_DEBUG

#include <cxxabi.h>
#include <stdlib.h>
#include <stdio.h>

#define DBGX( fmt, ... ) DBGX2( 0x1, fmt, ## __VA_ARGS__ )

#define DBGX2( flag, fmt, ... ) \
{\
    if ( flag & _DbgFlags ) {\
        char* realname = abi::__cxa_demangle(typeid(*this).name(),0,0,NULL);\
        fprintf( stderr, "%s::%s():%d: "fmt, realname ? realname : "?????????", \
                        __func__, __LINE__, ## __VA_ARGS__ );\
        fflush(stderr);\
        if ( realname ) free(realname);\
    }\
}

#define DBG( fmt, ... ) DBG2( 0x1, fmt, ## __VA_ARGS__ )

#define DBG2( flag, fmt, ... ) \
    if ( flag & _DbgFlags ) {\
    	fprintf( stderr, "%s():%d: "fmt, __func__, __LINE__, ##__VA_ARGS__);\
	}

#else

#define DBGX( fmt, args... )
#define DBGX2( flag, fmt, ... )
#define DBG2( fmt, args... )

#endif

#endif
