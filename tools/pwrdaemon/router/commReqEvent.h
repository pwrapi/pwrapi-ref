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

#ifndef _RTR_COMM_REQ_EVENT_H
#define _RTR_COMM_REQ_EVENT_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

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

    	std::vector< std::vector< ObjID > >& commList = 
				client.getCommList( commID );

    	CommReqInfo* info = new CommReqInfo;

    	DBGX("commID=%"PRIx64" eventId=%"PRIx64" new eventId=%p\n", 
													commID, id, info );

    	info->src = ec;
    	info->ev = this;
		info->grpInfo.resize( commList.size() );
		info->respQ.resize( commList.size() );
		info->pending = commList.size();
        info->resp = new CommRespEvent;
        info->resp->id = id;

    	id = (EventId) info;

		if ( op == Get ) {

			info->valueOp = valueOp;

			for ( size_t i = 0; i < valueOp.size(); i++ ) {
				DBGX("valueOp=%d\n",valueOp[i]);	
			}

        	static_cast<CommRespEvent*>(info->resp)->
                            timeStamp.resize( commList.size() );
        	static_cast<CommRespEvent*>(info->resp)->
                            value.resize( commList.size() );
   		}

    	for ( unsigned int i=0; i <  commList.size(); i++ ) {
			info->grpInfo[i] = commList[i].size();

			grpIndex = i;
    		for ( unsigned int j=0; j <  commList[i].size(); j++ ) {
				rtr.sendEvent( commList[i][j], this );
			}
    	}
		return false;
	}
};

}

#endif
