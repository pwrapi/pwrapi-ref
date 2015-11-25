
#ifndef _RTR_COMM_CREATE_EVENT_H
#define _RTR_COMM_CREATE_EVENT_H

#include <events.h>
#include <eventChannel.h>

namespace PWR_Router {

class RtrCommCreateEvent: public  CommCreateEvent {
  public:
   	RtrCommCreateEvent( SerialBuf& buf ) : CommCreateEvent( buf ) {}  

	bool process( EventGenerator* _rtr, EventChannel* ec );
};

}

#endif
