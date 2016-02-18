
#ifndef _SRVR_COMM_REQ_EVENT_H
#define _SRVR_COMM_REQ_EVENT_H

#include <events.h>
#include <debug.h>
#include "server.h"

namespace PWR_Server {

class SrvrCommReqEvent;
static void requestFini( SrvrCommReqEvent*, int status );

class SrvrCommReqEvent: public  CommReqEvent {
  public:
   	SrvrCommReqEvent( SerialBuf& buf ) : CommReqEvent( buf ), m_req(NULL) {
		DBGX("\n");
	}  

   	~SrvrCommReqEvent( ) {
		DBGX("\n");
		if ( m_req ) {
			PWR_ReqDestroy( m_req );
		}
	}

	bool process( EventGenerator* gen, EventChannel* ) {
		m_info = static_cast<Server*>(gen);

    	PWR_Obj obj = m_info->m_commMap[commID].objects[0];

		DBGX("commID=%lu\n",commID);
		char name[100];
		PWR_ObjGetName(obj,name,100);

   		DBGX("obj='%s':\n", name );
		for ( unsigned i = 0; i < attrName.size(); i++ ) {
    		DBGX("    attr=`%s`\n", PWR_AttrGetTypeString( attrName[i] ) );
		}
		m_respEvent.value.resize(1);
		m_respEvent.timeStamp.resize(1);
		m_respEvent.value[0].resize(attrName.size());
		m_respEvent.timeStamp[0].resize(attrName.size());
    	m_respEvent.op = op;
    	m_respEvent.id = id;
    	m_req = PWR_ReqCreateCallback( m_info->m_ctx, 
											(Callback)requestFini, this );
    	assert( m_req );

    	int ret;
    	if ( op == CommEvent::Get ) {
        	ret = PWR_ObjAttrGetValues_NB( obj, attrName.size(), 
				&attrName[0], &m_respEvent.value[0][0],
				&m_respEvent.timeStamp[0][0], m_req );
    	} else {
        	ret = PWR_ObjAttrSetValues_NB( obj, attrName.size(),
				&attrName[0], &values[0], m_req );
    	}

    	if ( ret != PWR_RET_SUCCESS ) {
        	requestFini( this, ret );
    	}

		return false;
	}

	Server*			m_info;
    CommRespEvent   m_respEvent;
	PWR_Request	    m_req;
};

static void requestFini( SrvrCommReqEvent* data, int status )
{
    DBG4("PWR_Server","status=%d\n",status);

    data->m_respEvent.status = status;

	data->m_info->fini( data, &data->m_respEvent );
}

}

#endif
