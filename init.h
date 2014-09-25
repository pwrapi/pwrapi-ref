
#ifndef _PWR_INIT_H
#define _PWR_INIT_H

#include "types.h"
class _Cntxt;

namespace PWR {

_Cntxt* init( PWR_CntxtType, PWR_Role, const char* );
int destroy( _Cntxt* );

}

#endif
