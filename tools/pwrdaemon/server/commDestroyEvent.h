
#ifndef _SRVR_COMM_DESTROY_EVENT_H
#define _SRVR_COMM_DESTROY_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h> 
#include "server.h"

namespace PWR_Server {

class SrvrCommDestroyEvent: public  CommDestroyEvent {
  public:
   	SrvrCommDestroyEvent( SerialBuf& buf ) : CommDestroyEvent( buf ) {
		DBG("\n");
	}  
	~SrvrCommDestroyEvent() {
		DBG("\n");
	}

	bool process( EventGenerator* gen, EventChannel* ec ) {
		Server& info = *static_cast<Server*>(gen);

       	DBG("commID=%llu\n", commID);

		// We have a bunch of PWR_Obj hanging off of the comm.
		// How/should we clean them up?	
		info.m_commMap.erase(commID);

		return true;
	}
};

}

#endif