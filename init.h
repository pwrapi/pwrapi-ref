
#ifndef PLI_INIT_H
#define PLI_INIT_H

#include "./types.h"

namespace PLI {

_Context* init( PLI_ContextType );
int destroy( PLI_Context );

}

#endif
