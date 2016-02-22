#ifndef _UTIL_H
#define _UTIL_H

#include <pwr.h>

static inline std::string getName( PWR_Grp grp, size_t index )
{
    char objName[100];
    int rc;
    PWR_Obj obj;
    rc = PWR_GrpGetObjByIndx( grp, index, &obj );
    assert( rc == PWR_RET_SUCCESS );

    rc = PWR_ObjGetName( obj, objName, 100 );
    assert( rc == PWR_RET_SUCCESS );
    return objName;
}

static inline PWR_AttrName getAttr( std::string name )
{
    if ( ! name.compare( "energy" ) ) {
        return PWR_ATTR_ENERGY;
    } else if ( ! name.compare( "power" ) ) {
        return PWR_ATTR_POWER;
    }
    assert(0);
}

#endif
