
#ifndef _RTR_COMM_REQ_EVENT_H
#define _RTR_COMM_REQ_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h>
#include "router.h"

namespace PWR_Router {

class RtrCommReqEvent: public  CommReqEvent {
  public:
   	RtrCommReqEvent( SerialBuf& buf ) : CommReqEvent( buf ) {}  

	bool process(EventGenerator* _rtr, EventChannel* ec) {
		Router& rtr = *static_cast<Router*>(_rtr);
		Router::Client& client = *rtr.getClient( ec );

    	CommReqInfo* info = new CommReqInfo;
    	info->src = ec;
    	info->ev = this;
    	info->id = id;
		info->valueOp = valueOp;

    	id = (EventId) info;

    	DBGX("commID=%llu eventId=%#llx new eventId=%p\n", commID, id, info );

    	std::vector<ObjID>& commList= client.getCommList( commID );

		info->num = commList.size(); 

    	std::vector<ObjID>::iterator iter = commList.begin();
    	for ( ; iter != commList.end(); ++iter ) {

        	DBGX("%s %d\n",(*iter).c_str(), attrName );
			rtr.sendEvent( (*iter), this );
    	}
		return false;
	}
};

}

#endif
