/* 
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#ifndef __DEBUG_H
#define __DEBUG_H


#ifdef DEBUG_ON  

#include <cxxabi.h>
#include <stdlib.h>
#include <stdio.h>

#define DBGX( fmt, ... ) \
{\
    if ( 1 ) {\
        char* realname = abi::__cxa_demangle(typeid(*this).name(),0,0,NULL);\
        fprintf( stderr, "%s::%s():%d: "fmt, realname ? realname : "?????????", \
                        __func__, __LINE__, ## __VA_ARGS__ );\
        fflush(stderr);\
        if ( realname ) free(realname);\
    }\
}

#define DBG( fmt, args... ) \
    fprintf( stderr, "%s():%d: "fmt, __func__, __LINE__, ##args)

#else

#define DBGX( fmt, args... )
#define DBG( fmt, args... )

#endif

#endif
