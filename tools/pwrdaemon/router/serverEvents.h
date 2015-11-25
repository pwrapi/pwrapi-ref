
#ifndef _RTR_SERVER_EVENTS_H
#define _RTR_SERVER_EVENTS_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h> 
#include "router.h"

namespace PWR_Router {

class RtrServerConnectEvent: public  ServerConnectEvent {
  public:
   	RtrServerConnectEvent( SerialBuf& buf ) : ServerConnectEvent( buf ) {}  

	bool process( EventGenerator* _rtr, EventChannel* ec ) {
        Router& rtr = *static_cast<Router*>(_rtr);
		rtr.addServer( name, ec );

//        Router::Server& server = *rtr.getServer( ec );
		DBGX("%s\n",name.c_str());
		return false;
	}
};

}

#endif

