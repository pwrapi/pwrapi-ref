#ifndef _RTR_RTR_ROUTER_EVENT_H
#define _RTR_RTR_ROUTER_EVENT_H

#include <eventChannel.h>
#include <debug.h>
#include "routerEvent.h"
#include "router.h"

namespace PWR_Router {

class RtrRouterEvent: public  RouterEvent {
  public:
    RtrRouterEvent( SerialBuf& buf ) : RouterEvent( buf ) {}

    bool process( EventGenerator* _rtr, EventChannel* ec ) {
        Router& rtr = *static_cast<Router*>(_rtr);
		DBGX("\n");
		rtr.sendEvent( dest, this );

        return false;
    }
};

}

#endif
