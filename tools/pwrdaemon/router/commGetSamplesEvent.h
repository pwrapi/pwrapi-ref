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

#ifndef _RTR_COMM_GET_SAMPLES_REQ_EVENT_H
#define _RTR_COMM_GET_SAMPLES_REQ_EVENT_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <events.h>
#include <eventChannel.h>
#include <debug.h>
#include "router.h"

namespace PWR_Router {

class RtrCommGetSamplesReqEvent: public  CommGetSamplesReqEvent {
  public:
   	RtrCommGetSamplesReqEvent( SerialBuf& buf ) : CommGetSamplesReqEvent( buf ) {}  

	bool process(EventGenerator* _rtr, EventChannel* ec) {
		Router& rtr = *static_cast<Router*>(_rtr);
		Router::Client& client = *rtr.getClient( ec );

		std::vector<std::vector< ObjID > >& commList= 
                client.getCommList( commID );

       // don't support more that one object at this time
        assert( 1 == commList.size() );
        assert( 1 == commList[0].size() );

    	CommReqInfo* info = new CommReqInfo;
    	info->src = ec;
    	info->ev = this;

        info->resp = new CommGetSamplesRespEvent;
        info->resp->id = id;
        id = (EventId) info;

    	DBGX("commID=%"PRIx64" eventId=%"PRIx64" new eventId=%p\n",
                                commID, id, info );

        for ( unsigned int i=0; i <  commList.size(); i++ ) {

            for ( unsigned int j=0; j <  commList[i].size(); j++ ) {
                rtr.sendEvent( commList[i][j], this );
            }
        }
		return false;
	}
};

class RtrCommGetSamplesRespEvent: public  CommGetSamplesRespEvent {
  public:
   	RtrCommGetSamplesRespEvent( SerialBuf& buf ) : CommGetSamplesRespEvent( buf ) {}  

	bool process(EventGenerator* _rtr, EventChannel* ec) {

        DBGX("id=%p status=%d \n",(void*)id, status );
        CommReqInfo* info = (CommReqInfo*) id;

        // Why don't we just sent this? Because we want to be consistent
        // CommRespEvent
        *static_cast<CommGetSamplesRespEvent*>(info->resp) = *this;
    
        info->src->sendEvent( info->resp );
        delete info->resp;
        delete info->ev;
        delete info;
		return false;
	}
};

}

#endif
