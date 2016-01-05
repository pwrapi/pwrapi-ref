
#include <assert.h>
#include <string>

#include <debug.h>

#include "allocEvent.h"
#include "routerEvent.h"
#include "commCreateEvent.h"
#include "commDestroyEvent.h"
#include "commGetSamplesReqEvent.h"
#include "commLogEvent.h"
#include "commReqEvent.h"

using namespace PWR_Server;

#ifdef USE_DEBUG
static const char* _eventNames[] = {
	    FOREACH_ENUM(GENERATE_STRING)
};
#endif

Event* PWR_Server::allocRtrEvent( unsigned int type, SerialBuf& buf )
{
	DBG4( "PWR_Server","`%s`\n",_eventNames[type]); 

	switch( (EventType) type ) {
	  default:
		assert(0);	
	  case Router2Router:
		return new SrvrRouterEvent( buf );
	}
	return NULL;
}

Event* PWR_Server::allocReqEvent( unsigned int type, SerialBuf& buf )
{
    DBG4("PWR_Server", "`%s`\n",_eventNames[type]);

    switch( (EventType) type ) {
      default:
        assert(0);
      case CommCreate:
        return new SrvrCommCreateEvent( buf );
      case CommDestroy:
        return new SrvrCommDestroyEvent( buf );
      case CommReq:
        return new SrvrCommReqEvent( buf );
      case CommLogReq:
        return new SrvrCommLogReqEvent( buf );
      case CommGetSamplesReq:
        return new SrvrCommGetSamplesReqEvent( buf );
    }
    return NULL;
}
