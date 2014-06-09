
#ifndef _PWR_UTIL_H
#define _PWR_UTIL_H

#include "./types.h"


static inline const char* attrTypeToString( PWR_AttrType name )
{
    switch( name ){
    case PWR_ATTR_NAME: return "Name";
    case PWR_ATTR_FREQ: return "Freq";
    case PWR_ATTR_PSTATE: return "Pstate";
    case PWR_ATTR_MAX_POWER: return "Max Power";
    case PWR_ATTR_MIN_POWER: return "Min Power";
    case PWR_ATTR_AVG_POWER: return "Avg Power";
    case PWR_ATTR_POWER: return "Power";
    case PWR_ATTR_VOLTAGE: return "Voltage";
    case PWR_ATTR_CURRENT: return "Current";
    case PWR_ATTR_ENERGY: return "Energy";
    case PWR_ATTR_INVALID: return "Invalid";
    default: return "????";
    }
    return NULL;
}


static inline const char* objTypeToString( PWR_ObjType type )
{
    switch( type ) {
    case PWR_OBJ_PLATFORM: return "Platform";
    case PWR_OBJ_CABINET:  return "Cabinet";
    case PWR_OBJ_BOARD:    return "Board";
    case PWR_OBJ_NODE:     return "Node";
    case PWR_OBJ_SOCKET:   return "Socket";
    case PWR_OBJ_CORE:     return "Core";
    case PWR_OBJ_NIC:      return "Nic";
    case PWR_OBJ_MEM:      return "Memory";
    case PWR_OBJ_INVALID:  return "Invalid";
    }
    return NULL;
}


static inline PWR_AttrType attrTypeStrToInt( const char* name )
{
    if ( 0 == strcmp(name,"POWER") ) {
        return PWR_ATTR_POWER;
    } else  if ( 0 == strcmp(name,"VOLTAGE") ) {
        return PWR_ATTR_VOLTAGE;
    } else  if ( 0 == strcmp(name,"CURRENT") ) {
        return PWR_ATTR_CURRENT;
    } else  if ( 0 == strcmp(name,"FREQ") ) {
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

static inline PWR_ObjType objTypeStrToInt( const char* name )
{ 
    if ( 0 == strcmp( name, "Platform" ) ) {
        return  PWR_OBJ_PLATFORM;
    } else if ( 0 == strcmp( name, "Cabinet" ) ) {
        return PWR_OBJ_CABINET;
    } else if ( 0 == strcmp( name, "Board" ) ) {
        return PWR_OBJ_BOARD;
    } else if ( 0 == strcmp( name, "Node" ) ) {
        return PWR_OBJ_NODE;
    } else if ( 0 == strcmp( name, "Socket" ) ) {
        return PWR_OBJ_SOCKET;
    } else if ( 0 == strcmp( name, "Core" ) ) {
        return PWR_OBJ_CORE;
    } 
    return PWR_OBJ_INVALID;
}

#endif
