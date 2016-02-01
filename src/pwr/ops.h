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

#ifndef _PWR_ATTRIBUTE_H
#define _PWR_ATTRIBUTE_H

#include <vector>
#include "debug.h"

namespace PowerAPI {

static void fpSum( int num, void* _out, void* _in )
{
    double* out = (double*) _out;
    double* in  = (double*) _in;
    *out = in[0];

    DBG("%s() i=%d val=%f\n",__func__, 0, in[0]);
    for ( int i = 1; i < num ;i++  ) {
        DBG("%s() i=%d val=%f\n",__func__, i, in[i]);
        *out += in[i];
    }
}

static void fpSum2( void* out, const std::vector<void*>& in  )
{
	DBG("%s() %lu\n",__func__,in.size());
    *((double*) out) = *((double*) in[0]);

    DBG("%s() i=%d val=%f\n",__func__, 0, *((double*) out));

    for ( unsigned int i=1; i < in.size(); i++ ) {
        DBG("%s() i=%d val=%f\n",__func__, i, *((double*) in[i]));
        *((double*) out) += *((double*) in[i]);
    }
}

static void fpAvg( int num, void* _out, void* _in )
{
    double* out = (double*) _out;
    double* in  = (double*) _in;
    *out = in[0];

    DBG("%s() i=%d val=%f\n",__func__, 0, in[0]);
    for ( int i = 1; i < num ;i++  ) {
        DBG("%s() i=%d val=%f\n",__func__, i, in[i]);
        *out += in[i];
    }
	*out /= num;
}

static void fpAvg2( void* out, const std::vector<void*>& in  )
{
	DBG("%s() %lu\n",__func__,in.size());
    *((double*) out) = *((double*) in[0]);

    DBG("%s() i=%d val=%f\n",__func__, 0, *((double*) out));

    for ( unsigned int i=1; i < in.size(); i++ ) {
        DBG("%s() i=%d val=%f\n",__func__, i, *((double*) in[i]));
        *((double*) out) += *((double*) in[i]);
    }
	*((double*) out) /= in.size();
}

}

#endif
