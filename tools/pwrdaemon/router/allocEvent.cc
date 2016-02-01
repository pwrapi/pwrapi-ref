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

#include <assert.h>
#include <string>

#include <debug.h>

#include "allocEvent.h"
#include "commCreateEvent.h"
#include "commReqEvent.h"
#include "commRespEvent.h"
#include "commLogEvents.h"
#include "commGetSamplesEvent.h"
#include "serverEvents.h"
#include "rtrRouterEvent.h"

using namespace PWR_Router;

#ifdef USE_DEBUG
static const char* _eventNames[] = {
	    FOREACH_ENUM(GENERATE_STRING)
};
#endif

Event* PWR_Router::allocRtrEvent( unsigned int type, SerialBuf& buf )
{
	DBG4("PWR_Router","`%s`\n",_eventNames[type]); 

	switch( (EventType) type ) {
	  default:
		assert(0);
	  case Router2Router:
		return new RtrRouterEvent( buf );
	}
	return NULL;
}

Event* PWR_Router::allocClientEvent( unsigned int type, SerialBuf& buf )
{
	DBG4("PWR_Router","`%s`\n",_eventNames[type]); 

	switch( (EventType) type ) {
	  default:
		assert(0);	
	  case CommCreate:
		return new RtrCommCreateEvent( buf );
	  case CommReq:
		return new RtrCommReqEvent( buf );
	  case CommLogReq:
		return new RtrCommLogReqEvent( buf );
	  case CommGetSamplesReq:
		return new RtrCommGetSamplesReqEvent( buf );
	}
	return NULL;
}

Event* PWR_Router::allocServerEvent( unsigned int type, SerialBuf& buf )
{
	DBG("`%s`\n",_eventNames[type]); 

	switch( (EventType) type ) {
	  default:
		assert(0);	
	  case Router2Router:
		return new RtrRouterEvent( buf );
	  case CommResp:
		return new RtrCommRespEvent( buf );
	  case CommLogResp:
		return new RtrCommLogRespEvent( buf );
	  case CommGetSamplesResp:
		return new RtrCommGetSamplesRespEvent( buf );
	  case ServerConnect:
		return new RtrServerConnectEvent( buf );
	}
	return NULL;
}

