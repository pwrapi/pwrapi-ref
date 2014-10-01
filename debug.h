#ifndef _PWR_DEBUG_H
#define _PWR_DEBUG_H

#ifdef DEBUG_ON  

#include <cxxabi.h>
#include <stdlib.h>
#include <stdio.h>

#define DBGX( fmt, args... ) \
{\
     char* realname = abi::__cxa_demangle(typeid(*this).name(),0,0,NULL);\
    fprintf( stderr, "%s::%s():%d: "fmt, realname ? realname : "?????????", \
                        __func__, __LINE__, ##args);\
    fflush(stderr);\
    if ( realname ) free(realname);\
}

#define DBG( fmt, args... ) \
    fprintf( stderr, "%s():%d: "fmt, __func__, __LINE__, ##args)

#else

#define DBGX( fmt, args... )
#define DBG( fmt, args... )

#endif



#endif
