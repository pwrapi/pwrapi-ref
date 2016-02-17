/*
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#ifndef _RTR_COMM_LOG_EVENT_H
#define _RTR_COMM_LOG_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h> 
#include "router.h"

namespace PWR_Router {

class RtrCommLogReqEvent: public  CommLogReqEvent {
  public:
   	RtrCommLogReqEvent( SerialBuf& buf ) : CommLogReqEvent( buf ) {}  

	bool process( EventGenerator* _rtr, EventChannel* ec ) {
        Router& rtr = *static_cast<Router*>(_rtr);
		Router::Client& client = *rtr.getClient( ec );

        CommReqInfo* info = new CommReqInfo;
        info->src = ec;
        info->ev = this;
        info->id = id;

        id = (EventId) info;

        DBGX("commID=%llu eventId=%#llx new eventId=%p\n",
                                commID, id, info );

		std::vector<ObjID>& commList= client.getCommList( commID );
        std::vector<ObjID>::iterator iter = commList.begin();

        for ( ; iter != commList.end(); ++iter ) {

            DBGX("%s %d\n",(*iter).c_str(), attrName );
			rtr.sendEvent( (*iter), this );
        }
		return false;
	}
};

class RtrCommLogRespEvent: public  CommLogRespEvent {
  public:
   	RtrCommLogRespEvent( SerialBuf& buf ) : CommLogRespEvent( buf ) {}  

	bool process( EventGenerator* _rtr, EventChannel* ec ) {

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
