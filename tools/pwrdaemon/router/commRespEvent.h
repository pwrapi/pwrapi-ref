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

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

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
   	RtrCommRespEvent( SerialBuf& buf ) : CommRespEvent( buf ){ }  

	bool process( EventGenerator* _rtr, EventChannel* ) {

		DBGX("id=%p status=%"PRIi32" grpIndex=%" PRIu64 "\n",
								(void*)id, status, grpIndex );

        CommReqInfo* info = (CommReqInfo*) id;

		info->respQ[grpIndex].push_back( this );

		if ( info->respQ[grpIndex].size() == info->grpInfo[grpIndex] ) {

            CommRespEvent* resp = static_cast<CommRespEvent*>(info->resp);
			if ( Get == info->ev->op ) {
				DBGX("index %"PRIu64" is ready, num attrs %zu\n",
										grpIndex, info->valueOp.size() );
				resp->timeStamp[grpIndex].resize( info->valueOp.size() );
				resp->value[grpIndex].resize( info->valueOp.size() );

				for ( unsigned i = 0; i < info->valueOp.size(); i++ ) { 
					resp->value[grpIndex][i] = 0;
				}

				for ( unsigned j = 0; j < info->respQ[grpIndex].size(); j++ ) {
					for ( unsigned i = 0; i < info->valueOp.size(); i++ ) { 
						DBGX( "op=%d \n", info->valueOp[i] );			
						assert( FP_ADD == info->valueOp[i] );

						fpAdd( &resp->value[grpIndex][i], 
									&info->respQ[grpIndex][j]->value[0][i] );

						resp->timeStamp[grpIndex][i] = 
									info->respQ[grpIndex][j]->timeStamp[0][i];
					}
				} 
			}
			for ( unsigned j = 0; j < info->respQ[grpIndex].size(); j++ ) {
				resp->errValue.insert( resp->errValue.end(), 
							info->respQ[grpIndex][j]->errValue.begin(), 
							info->respQ[grpIndex][j]->errValue.end() );
				resp->errAttr.insert( resp->errAttr.end(), 
							info->respQ[grpIndex][j]->errAttr.begin(), 
							info->respQ[grpIndex][j]->errAttr.end() );
				resp->errObj.insert( resp->errObj.end(), 
							info->respQ[grpIndex][j]->errObj.begin(), 
							info->respQ[grpIndex][j]->errObj.end() );

				if ( j < info->respQ[grpIndex].size() - 1 ) {
					delete info->respQ[grpIndex][ j ];
				}
			}
			// quiet valgrind
			resp->grpIndex = 0;
			resp->commID = 0;

			DBGX("pending %zu\n",info->pending);
			--info->pending;

			if ( 0 == info->pending ) {
				DBGX("done send the response\n");
        		info->src->sendEvent( info->resp );
				delete info->resp;
				delete info->ev;
				delete info;
			} 

			return true; 

		} else {
			return false;
		} 
	}
};

}

#endif
