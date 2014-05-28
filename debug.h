#ifndef _PWR_DEBUG_H
#define _PWR_DEBUG_H



#if 1 

#include <cxxabi.h>

#define DBGX( fmt, args... ) \
{\
     char* realname = abi::__cxa_demangle(typeid(*this).name(),0,0,NULL);\
    fprintf( stderr, "%s::%s():%d: "fmt, realname ? realname : "?????????", \
                        __func__, __LINE__, ##args);\
    fflush(stderr);\
    if ( realname ) free(realname);\
}

#else

#define DBGX( fmt, args... )

#endif



#endif
