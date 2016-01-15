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

#include "cntxt.h"
#include "debug.h"

using namespace PowerAPI;

extern "C" {
	getDevFuncPtr_t PowerInsight_getDev(); 
}

plugin_dev_t* Cntxt::getDev( std::string lib, std::string name )
{
    DBGX("lib %s name=`%s`\n", lib.c_str(), name.c_str() );
#if 0
#ifdef  HAVE_POWER_INSIGHT	
	if ( ! name.compare("PowerInsight") ) {
		getDevFuncPtr_t tmp = PowerInsight_getDev;
		return tmp; 
	}
#endif
#endif
    return NULL;
}
