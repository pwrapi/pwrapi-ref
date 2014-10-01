
#ifndef _PWR_UTIL_H
#define _PWR_UTIL_H

#include <string.h>
#include <sstream>

#include "types.h"

class NumToString : public std::string  {
  public:
	NumToString( uint64_t num ) {
		std::stringstream tmpSS;
    	tmpSS << num;
		append( tmpSS.str() );
	}
};   

class StringToNum {
  public:
	StringToNum( const std::string str ) {
	    std::stringstream( str ) >> num;
	}
	uint64_t uint64() {
		return num;
	}
  private:
	uint64_t  num;
};

static inline std::string objTypeToString( PWR_ObjType type )
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

static inline std::string attrNameToString( PWR_AttrName name )
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

#endif
