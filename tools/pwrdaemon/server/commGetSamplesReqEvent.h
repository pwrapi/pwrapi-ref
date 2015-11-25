
#ifndef _SRVR_COMM_GET_SAMPLES_REQ_EVENT_H
#define _SRVR_COMM_GET_SAMPLES_REQ_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h>
#include "server.h"


namespace PWR_Server {

class SrvrCommGetSamplesReqEvent;
static int getSamplesFini( SrvrCommGetSamplesReqEvent*, int status );

class SrvrCommGetSamplesReqEvent: public  CommGetSamplesReqEvent {
  public:
   	SrvrCommGetSamplesReqEvent( SerialBuf& buf ) : CommGetSamplesReqEvent( buf ) {}  
   	~SrvrCommGetSamplesReqEvent( ) {
		DBGX("\n");
	}

	bool process( EventGenerator* gen, EventChannel* ec) {
		m_info = static_cast<Server*>(gen);

    	PWR_Obj obj = m_info->m_commMap[commID].objects[0];

		DBGX("%llu\n",commID);

    	DBGX("obj='%s' attr=`%s`\n", PWR_ObjGetName(obj),
                            PWR_AttrGetTypeString( attrName ) );
		DBGX("period=%f count=%d\n", period, count );
    	m_evChan = ec;
    	m_respEvent.id = id;
		m_respEvent.data.resize( count );
		m_respEvent.count = count;
    	m_req = PWR_ReqCreateCallback( m_info->m_ctx, 
											(Callback)getSamplesFini, this );
    	assert( m_req );
			
    	int ret = PWR_ObjAttrGetSamples_NB( obj, attrName,
           		&m_respEvent.startTime, period, &m_respEvent.count, 
				(void*) &m_respEvent.data[0], m_req );

    	if ( ret != PWR_RET_SUCCESS ) {
        	getSamplesFini( this, ret );
    	}

		return false;
	}

    CommGetSamplesRespEvent   m_respEvent;

	Server*		m_info;
    EventChannel*   m_evChan;
	PWR_Request	    m_req;
};

static int getSamplesFini( SrvrCommGetSamplesReqEvent* data, int status )
{
    DBG("status=%d\n",status);

    data->m_respEvent.status = status;

    RouterEvent* ev = new RouterEvent( 0, 0, &data->m_respEvent, true );

    data->m_evChan->sendEvent( ev );

	delete data;
	
    return 1;
}

}

#endif
