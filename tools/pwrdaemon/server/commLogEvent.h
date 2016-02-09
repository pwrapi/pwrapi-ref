
#ifndef _SRVR_COMM_LOG_EVENT_H
#define _SRVR_COMM_LOG_EVENT_H

#include <events.h>
#include <debug.h> 
#include "server.h"

namespace PWR_Server {

class SrvrCommLogReqEvent;
static void logReqFini( SrvrCommLogReqEvent* data, int status );

class SrvrCommLogReqEvent: public  CommLogReqEvent {
  public:
   	SrvrCommLogReqEvent( SerialBuf& buf ) : CommLogReqEvent( buf ), m_req(NULL) {  
		DBGX("\n");
    }

    ~SrvrCommLogReqEvent( ) {
        DBGX("\n");
		if ( m_req ) { 
			PWR_ReqDestroy( m_req );
		}
    }

	bool process( EventGenerator* gen, EventChannel* ) {
        m_info = static_cast<Server*>(gen);

		DBGX("commID=%lu\n",commID);
		PWR_Obj obj = m_info->m_commMap[commID].objects[0];

		const char* name;
		PWR_ObjGetName(obj,&name);
		DBGX("obj='%s' attr=`%s`\n", name, PWR_AttrGetTypeString( attrName ) );
        m_respEvent.op = op;
        m_respEvent.id = id;

        m_req = PWR_ReqCreateCallback( m_info->m_ctx,
                                            (Callback)logReqFini, this );
        assert( m_req );

        int ret;
        if ( op == CommEvent::Start ) {
            ret = PWR_ObjAttrStartLog_NB( obj, attrName, m_req );
        } else {
            ret = PWR_ObjAttrStopLog_NB( obj, attrName, m_req );
        }

        if ( ret != PWR_RET_SUCCESS ) {
            logReqFini( this, ret );
        }

		return false;
	}

	PWR_Request			m_req;
	Server* 			m_info;
	CommLogRespEvent 	m_respEvent;
};

static void logReqFini( SrvrCommLogReqEvent* data, int status )
{
	DBG4("PWR_Server","status=%d\n",status);

    data->m_respEvent.status = status;

	data->m_info->fini( data, &data->m_respEvent );
}

}

#endif
