
#ifndef _PWR_INIT_H
#define _PWR_INIT_H

#include "types.h"
class Cntxt;

namespace PWR {

Cntxt* init( PWR_CntxtType, PWR_Role, const char* );
int destroy( Cntxt* );

}

#endif
