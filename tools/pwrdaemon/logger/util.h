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

static inline PWR_AttrStat getAttrStat( std::string name )
{
    if ( ! name.compare( "min" ) ) {
        return PWR_ATTR_STAT_MIN;
    } else if ( ! name.compare( "max" ) ) {
        return PWR_ATTR_STAT_MAX;
    } else if ( ! name.compare( "avg" ) ) {
        return PWR_ATTR_STAT_AVG;
    } else if ( ! name.compare( "stdev" ) ) {
        return PWR_ATTR_STAT_STDEV;
    } else if ( ! name.compare( "cv" ) ) {
        return PWR_ATTR_STAT_CV;
    } else if ( ! name.compare( "sum" ) ) {
        return PWR_ATTR_STAT_SUM;
    }
    assert(0);
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
