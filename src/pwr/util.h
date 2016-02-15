/* 
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#ifndef _PWR_UTIL_H
#define _PWR_UTIL_H

#include "pwrtypes.h"

static inline const char* objTypeToString( PWR_ObjType type )
{
    switch( type ) {
    case PWR_OBJ_PLATFORM:    return "Platform";
    case PWR_OBJ_CABINET:     return "Cabinet";
    case PWR_OBJ_CHASSIS:     return "Chassis";
    case PWR_OBJ_BOARD:       return "Board";
    case PWR_OBJ_NODE:        return "Node";
    case PWR_OBJ_SOCKET:      return "Socket";
    case PWR_OBJ_CORE:        return "Core";
    case PWR_OBJ_POWER_PLANE: return "Power Plane";
    case PWR_OBJ_MEM:         return "Memory";
    case PWR_OBJ_NIC:         return "Nic";
    case PWR_OBJ_INVALID:     return "Invalid";
    default: return "????";
    }
    return NULL;
}

static inline const char* attrNameToString( PWR_AttrName name )
{
    switch( name ){
    case PWR_ATTR_PSTATE:          return "P-State";
    case PWR_ATTR_CSTATE:          return "C-State";
    case PWR_ATTR_SSTATE:          return "S-State";
    case PWR_ATTR_CURRENT:         return "Current";
    case PWR_ATTR_VOLTAGE:         return "Voltage";
    case PWR_ATTR_POWER:           return "Power";
    case PWR_ATTR_POWER_LIMIT_MIN: return "Power Limit Min";
    case PWR_ATTR_POWER_LIMIT_MAX: return "Power Limit Max";
    case PWR_ATTR_FREQ:            return "Freq";
    case PWR_ATTR_FREQ_LIMIT_MIN:  return "Freq Limit Min";
    case PWR_ATTR_FREQ_LIMIT_MAX:  return "Freq Limit Max";
    case PWR_ATTR_ENERGY:          return "Energy";
    case PWR_ATTR_TEMP:            return "Temp";
    case PWR_ATTR_THROTTLED_TIME:  return "Throttled Time";
    case PWR_ATTR_THROTTLED_COUNT: return "Throttled Count";
    case PWR_ATTR_INVALID:         return "Invalid";
    default: return "????";
    }
    return NULL;
}

static inline const char* attrUnitsToString( PWR_AttrName name )
{
    switch( name ){
    case PWR_ATTR_POWER:           return "Watts";
    case PWR_ATTR_ENERGY:          return "Joules";
    case PWR_ATTR_INVALID:         return "Invalid";
    default: return "????";
    }
    return NULL;
}

static inline const char* attrStatToString( PWR_AttrStat stat )
{
	switch( stat ) {
    case PWR_ATTR_STAT_MIN:     return "Min";
    case PWR_ATTR_STAT_MAX:     return "Max";
    case PWR_ATTR_STAT_AVG:     return "Avg"; 
    case PWR_ATTR_STAT_STDEV:   return "Stdev";
    case PWR_ATTR_STAT_CV:      return "CV";
    case PWR_ATTR_STAT_INVALID: return "Invalid";
    default: return "????";
	}
	return NULL;
}

#endif
