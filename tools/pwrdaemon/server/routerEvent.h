
#ifndef _SRVR_ROUTER_EVENT_H
#define _SRVR_ROUTER_EVENT_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <events.h>
#include <eventChannel.h>
#include <debug.h> 
#include "allocEvent.h"
#include "../router/routerEvent.h"
#include "server.h"

namespace PWR_Server {

class SrvrRouterEvent: public RouterEvent {
  public:
   	SrvrRouterEvent( SerialBuf& buf ) : RouterEvent( buf ) {
		DBGX("\n");
	}  
	~SrvrRouterEvent() {
		DBGX("\n");
	}  

	bool process( EventGenerator* gen, EventChannel* ec ) {

        Server* info = static_cast<Server*>(gen);

        DBGX("src=%" PRIx64 " dest=%" PRIx64 "\n",src,dest);
        Event* ev = getPayload( allocReqEvent );

       	info->initFini( ev, this, ec );

        if ( ev->process( gen ) ) {
            delete ev;
			info->freeFini( ev );
			return true;
        } else {
        	return false;
		}

    }
};

};

#endif
