
#ifndef _RTR_COMM_RESP_EVENT_H
#define _RTR_COMM_RESP_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h>
#include "router.h"

namespace PWR_Router {

class RtrCommRespEvent: public  CommRespEvent {
  public:
   	RtrCommRespEvent( SerialBuf& buf ) : CommRespEvent( buf ) {}  

	bool process( EventGenerator* _rtr, EventChannel*) {

		DBGX("id=%p status=%d \n",(void*)id, status );

        CommReqInfo* info = (CommReqInfo*) id;

        id = info->id;
        info->src->sendEvent( this );
        delete info;

		return true;
	}
};

}

#endif
