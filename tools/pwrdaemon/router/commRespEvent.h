
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
			resp->value = 0;

			DBGX( "op=%d \n", info->valueOp );			
			assert( FP_ADD == info->valueOp );
			std::deque<CommRespEvent*>::iterator iter = info->respQ.begin();	
			for ( ; iter != info->respQ.end(); ++iter ) {
        		delete (*iter);
				fpAdd( &resp->value, &(*iter)->value );
				resp->timeStamp = (*iter)->timeStamp;
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
