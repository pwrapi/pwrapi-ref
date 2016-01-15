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

#include <events.h>
#include <assert.h>
#include "debug.h"
#include "events.h"

#ifdef USE_DEBUG
static const char* _eventNames[] = {
	    FOREACH_ENUM(GENERATE_STRING)
};
#endif

Event* ctx_allocEvent( unsigned int type, SerialBuf& buf )
{
	DBG("`%s`\n",_eventNames[type]); 

	switch( (EventType) type ) {
	  default:
		assert(0);	
	  case CommResp:
		return new CommRespEvent( buf );
	  case CommLogResp:
		return new CommLogRespEvent( buf );
	  case CommGetSamplesResp:
		return new CommGetSamplesRespEvent( buf );
	}
	return NULL;
}
