
#ifndef _SRVR_COMM_CREATE_EVENT_H
#define _SRVR_COMM_CREATE_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h> 
#include "server.h"

namespace PWR_Server {

class SrvrCommCreateEvent: public  CommCreateEvent {
  public:
   	SrvrCommCreateEvent( SerialBuf& buf ) : CommCreateEvent( buf ) {
		DBG("\n");
	}  
	~SrvrCommCreateEvent() {
		DBG("\n");
	}  

	bool process( EventGenerator* gen, EventChannel* ec ) {
		Server& info = *static_cast<Server*>(gen);

       	DBG("commID=%llu\n", commID);

    	CommInfo& cInfo = info.m_commMap[commID];
    	cInfo.objects.resize( members.size() );
    	assert( 1 == members.size() );

    	for ( unsigned int i = 0; i < members.size(); i++ ) {
        	std::string& name = members[i];
        	DBG("get object %s\n", name.c_str());
        	cInfo.objects[i] = PWR_CntxtGetObjByName( 
												info.m_ctx, name.c_str() );
        	assert( cInfo.objects[i] );
   		}	
		return true;
	}
};

}

#endif
