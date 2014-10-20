#ifndef _PWR_DEBUG_H
#define _PWR_DEBUG_H

extern unsigned int _DbgFlags;

#define DBG_CONFIG (1<<1)

#ifdef DEBUG_ON  

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

#define DBG( fmt, args... ) \
    fprintf( stderr, "%s():%d: "fmt, __func__, __LINE__, ##args)

#else

#define DBGX( fmt, args... )
#define DBGX2( flag, fmt, ... )
#define DBG( fmt, args... )

#endif

#endif
