
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
		DBGX("\n");
	}  
	~SrvrCommCreateEvent() {
		DBGX("\n");
	}  

	bool process( EventGenerator* gen, EventChannel* ) {
		Server& info = *static_cast<Server*>(gen);

       	DBGX("commID=%"PRIx64"\n", commID);

    	CommInfo& cInfo = info.m_commMap[commID];
    	cInfo.objects.resize( members.size() );
    	assert( 1 == members[0].size() );

    	for ( unsigned int i = 0; i < members[0].size(); i++ ) {
        	std::string& name = members[0][i];
        	DBGX("get object %s\n", name.c_str());
        	PWR_CntxtGetObjByName(info.m_ctx, name.c_str(), &(cInfo.objects[i]) );
        	assert( cInfo.objects[i] );
   		}	
		return true;
	}
};

}

#endif
