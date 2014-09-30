#include "objTreeNode.h"
#include "cntxt.h"

namespace PWR {

Cntxt* init( PWR_CntxtType type, PWR_Role role, const char* name )
{
    return new Cntxt( type, role, name ); 
}

int destroy( Cntxt* ctx )
{
    delete ctx; 
    return PWR_RET_SUCCESS;
}

}
