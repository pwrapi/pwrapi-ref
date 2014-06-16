
#ifndef _PWR_INIT_H
#define _PWR_INIT_H

#include "./types.h"

namespace PWR {

_Cntxt* init( PWR_CntxtType, PWR_Role, const char* );
int destroy( PWR_Cntxt );

}

#endif
