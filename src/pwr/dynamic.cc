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

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "distCntxt.h"
#include "debug.h"
#include "pwrdev.h"

using namespace PowerAPI;

plugin_dev_t* DistCntxt::getDev( std::string lib, std::string name )
{
	DBGX("lib %s name=`%s`\n", lib.c_str(), name.c_str() );
    void* ptr = dlopen( lib.c_str(), RTLD_LAZY);
    if ( NULL == ptr ) {
        printf("error: can't find plugin library `%s`\n", lib.c_str());
        exit(-1);
    }

    getDevFuncPtr_t funcPtr = (getDevFuncPtr_t) dlsym( ptr, GETDEVFUNC ); 
	assert(funcPtr);
	return funcPtr();
}
