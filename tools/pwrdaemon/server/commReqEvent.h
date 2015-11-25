
#ifndef _SRVR_COMM_REQ_EVENT_H
#define _SRVR_COMM_REQ_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h>
#include "server.h"

namespace PWR_Server {

class SrvrCommReqEvent;
static int requestFini( SrvrCommReqEvent*, int status );

class SrvrCommReqEvent: public  CommReqEvent {
  public:
   	SrvrCommReqEvent( SerialBuf& buf ) : CommReqEvent( buf ) {
		DBGX("\n");
	}  

   	~SrvrCommReqEvent( ) {
		DBGX("\n");
		if ( m_req ) {
			PWR_ReqDestroy( m_req );
		}
	}

	bool process( EventGenerator* gen, EventChannel* ec) {
		m_info = static_cast<Server*>(gen);

    	PWR_Obj obj = m_info->m_commMap[commID].objects[0];

		DBGX("commID=%llu\n",commID);
    	DBGX("obj='%s' attr=`%s`\n", PWR_ObjGetName(obj),
                            PWR_AttrGetTypeString( attrName ) );
    	m_evChan = ec;
    	m_respEvent.op = op;
    	m_respEvent.id = id;
    	m_req = PWR_ReqCreateCallback( m_info->m_ctx, 
											(Callback)requestFini, this );
    	assert( m_req );

    	int ret;
    	if ( op == CommEvent::Get ) {
        	ret = PWR_ObjAttrGetValue_NB( obj, attrName,
           		&m_respEvent.value, &m_respEvent.timeStamp, m_req );
    	} else {
        	ret = PWR_ObjAttrSetValue_NB( obj, attrName, &value, m_req );
    	}

    	if ( ret != PWR_RET_SUCCESS ) {
        	requestFini( this, ret );
    	}

		return false;
	}

	Server*		m_info;
    CommRespEvent   m_respEvent;
    EventChannel*   m_evChan;
	PWR_Request	    m_req;
};

static int requestFini( SrvrCommReqEvent* data, int status )
{
    DBG("status=%d\n",status);

    data->m_respEvent.status = status;
	RouterEvent* ev = new RouterEvent( 0,0, &data->m_respEvent, true );

    data->m_evChan->sendEvent( ev );
	
    return 1;
}

}

#endif
