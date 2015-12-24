
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

static const char* _eventNames[] = {
	    FOREACH_ENUM(GENERATE_STRING)
};

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

