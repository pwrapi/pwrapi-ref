#include "./object.h"

namespace PWR {

_Cntxt* init( PWR_CntxtType type, PWR_Role role, const char* name )
{
    return new _Cntxt( type, role, name ); 
}

int destroy( _Cntxt* ctx )
{
    delete ctx; 
    return PWR_ERR_SUCCESS;
}

}
