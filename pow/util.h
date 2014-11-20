/* 
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
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

#include "types.h"

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

static inline const char* attrNameToString( PWR_AttrName name )
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

static inline const char* attrStatToString( PWR_AttrStat stat )
{
	switch( stat ) {
    case PWR_ATTR_STAT_MIN: return "Min";
    case PWR_ATTR_STAT_MAX: return "Max";
    case PWR_ATTR_STAT_AVG: return "Avg"; 
    case PWR_ATTR_STAT_STDEV: return "Stdev";
    case PWR_ATTR_STAT_MEAN: return "Mean";
    case PWR_ATTR_STAT_CV: return "CV";
    default: return "????";
	}
	return NULL;
}

#endif
