
#ifndef _SRVR_COMM_GET_SAMPLES_REQ_EVENT_H
#define _SRVR_COMM_GET_SAMPLES_REQ_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h>
#include "server.h"


namespace PWR_Server {

class SrvrCommGetSamplesReqEvent;
static void getSamplesFini( SrvrCommGetSamplesReqEvent*, int status );

class SrvrCommGetSamplesReqEvent: public  CommGetSamplesReqEvent {
  public:
   	SrvrCommGetSamplesReqEvent( SerialBuf& buf ) : 
		CommGetSamplesReqEvent( buf ), m_req(NULL) {}  

   	~SrvrCommGetSamplesReqEvent( ) {
		DBGX("\n");
		if ( m_req ) {
			PWR_ReqDestroy( m_req );
		}
	}

	bool process( EventGenerator* gen, EventChannel* ) {
		m_info = static_cast<Server*>(gen);

    	PWR_Obj obj = m_info->m_commMap[commID].objects[0];

		DBGX("%lu\n",commID);

		const char* name;
		PWR_ObjGetName(obj,&name);
    	DBGX("obj='%s' attr=`%s`\n", name,
                            PWR_AttrGetTypeString( attrName ) );
		DBGX("period=%f count=%d\n", period, count );
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

	Server*			m_info;
	PWR_Request	    m_req;
};

static void getSamplesFini( SrvrCommGetSamplesReqEvent* data, int status )
{
    DBG4("PWR_Server","status=%d\n",status);

    data->m_respEvent.status = status;

	data->m_info->fini( data, &data->m_respEvent );
}

}

#endif
