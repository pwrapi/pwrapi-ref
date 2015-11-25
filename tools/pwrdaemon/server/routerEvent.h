
#ifndef _SRVR_ROUTER_EVENT_H
#define _SRVR_ROUTER_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h> 
#include "server.h"
#include "../router/routerEvent.h"

namespace PWR_Server {

class SrvrRouterEvent: public  RouterEvent {
  public:
   	SrvrRouterEvent( SerialBuf& buf ) : RouterEvent( buf ) {
		DBG("\n");
	}  
	~SrvrRouterEvent() {
		DBG("\n");
	}  

	bool process( EventGenerator* gen, EventChannel* ec ) {

		DBGX("\n");
		Event* ev = getPayload( allocReqEvent );
		if ( ev->process( gen, ec ) ) {
			delete ev;
		}	

		return true;
	}
};

};

#endif
