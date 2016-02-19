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

#ifndef _RTR_COMM_RESP_EVENT_H
#define _RTR_COMM_RESP_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h>
#include "router.h"

namespace PWR_Router {

static inline void fpAdd( void* inout, void* in )
{
	*(double*)inout += *(double*)in;
}

class RtrCommRespEvent: public  CommRespEvent {
  public:
   	RtrCommRespEvent( SerialBuf& buf ) : CommRespEvent( buf ) {}  

	bool process( EventGenerator* _rtr, EventChannel* ) {

		DBGX("id=%p status=%d \n",(void*)id, status );

        CommReqInfo* info = (CommReqInfo*) id;

		info->respQ.push_back( this );

		if ( info->respQ.size() == info->num ) {

			CommRespEvent* resp = new CommRespEvent;
        	resp->id = info->id;
			resp->timeStamp.resize(1);
			resp->value.resize(1);
			resp->timeStamp[0].resize( info->valueOp.size() );
			resp->value[0].resize( info->valueOp.size() );
			for ( unsigned i = 0; i < info->valueOp.size(); i++ ) { 
				resp->value[0][i] = 0;
			}
			DBGX("num attrs %lu\n",info->valueOp.size() );

			std::deque<CommRespEvent*>::iterator iter = info->respQ.begin();	
			for ( ; iter != info->respQ.end(); ++iter ) {
				for ( unsigned i = 0; i < info->valueOp.size(); i++ ) { 
					DBGX( "op=%d \n", info->valueOp[i] );			
					assert( FP_ADD == info->valueOp[i] );
					fpAdd( &resp->value[0][i], &(*iter)->value[0][i] );
					resp->timeStamp[0][i] = (*iter)->timeStamp[0][i];
				}
				delete (*iter);
			} 

        	info->src->sendEvent( resp );

			delete info;

			return false;

		} else {
			return false;
		} 
	}
};

}

#endif
