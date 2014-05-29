
#ifndef _PWR_UTIL_H
#define _PWR_UTIL_H

#include "./types.h"
#include <string>

static inline PWR_AttrType attrTypeStrToInt( const std::string name )
{
    if ( 0 == name.compare("POWER") ) {
        return PWR_ATTR_POWER;
    } else  if ( 0 == name.compare("VOLTAGE") ) {
        return PWR_ATTR_VOLTAGE;
    } else  if ( 0 == name.compare("CURRENT") ) {
        return PWR_ATTR_CURRENT;
    } else  if ( 0 == name.compare("FREQ") ) {
        return PWR_ATTR_FREQ;
    }
    return PWR_ATTR_INVALID;
}

static inline PWR_AttrDataType attrTypeToDataType( PWR_AttrType type )
{
    switch ( type ) {
    case PWR_ATTR_NAME:
        return PWR_ATTR_DATA_STRING;

    case PWR_ATTR_PSTATE:
        return PWR_ATTR_DATA_INT;

    case PWR_ATTR_FREQ:
    case PWR_ATTR_MAX_POWER:
    case PWR_ATTR_MIN_POWER:
    case PWR_ATTR_AVG_POWER:
    case PWR_ATTR_ENERGY:
    case PWR_ATTR_POWER:
    case PWR_ATTR_VOLTAGE:
    case PWR_ATTR_CURRENT:
        return PWR_ATTR_DATA_FLOAT;
    case PWR_ATTR_INVALID:
        break;
    default:
        assert(0);
    }
    return PWR_ATTR_DATA_INVALID;
}

static inline PWR_ObjType objTypeStrToInt( const std::string name )
{ 
    if ( 0 == name.compare( "Platform" ) ) {
        return  PWR_OBJ_PLATFORM;
    } else if ( 0 == name.compare( "Cabinet" ) ) {
        return PWR_OBJ_CABINET;
    } else if ( 0 == name.compare( "Board" ) ) {
        return PWR_OBJ_BOARD;
    } else if ( 0 == name.compare( "Node" ) ) {
        return PWR_OBJ_NODE;
    } else if ( 0 == name.compare( "Socket" ) ) {
        return PWR_OBJ_SOCKET;
    } else if ( 0 == name.compare( "Core" ) ) {
        return PWR_OBJ_CORE;
    } 
    return PWR_OBJ_INVALID;
}

#endif
