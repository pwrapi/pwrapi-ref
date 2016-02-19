
#ifndef _SRVR_COMM_DESTROY_EVENT_H
#define _SRVR_COMM_DESTROY_EVENT_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <events.h>
#include <eventChannel.h>
#include <debug.h> 
#include "server.h"

namespace PWR_Server {

class SrvrCommDestroyEvent: public  CommDestroyEvent {
  public:
   	SrvrCommDestroyEvent( SerialBuf& buf ) : CommDestroyEvent( buf ) {
		DBGX("\n");
	}  
	~SrvrCommDestroyEvent() {
		DBGX("\n");
	}

	bool process( EventGenerator* gen, EventChannel* ) {
		Server& info = *static_cast<Server*>(gen);

       	DBGX("commID=%"PRIx64"\n", commID);

		// We have a bunch of PWR_Obj hanging off of the comm.
		// How/should we clean them up?	
		info.m_commMap.erase(commID);

		return true;
	}
};

}

#endif
