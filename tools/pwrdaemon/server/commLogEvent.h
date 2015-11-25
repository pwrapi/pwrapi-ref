
#ifndef _SRVR_COMM_LOG_EVENT_H
#define _SRVR_COMM_LOG_EVENT_H

#include <events.h>
#include <eventChannel.h>
#include <debug.h> 
#include "server.h"
#include "debug.h"

namespace PWR_Server {

class SrvrCommLogReqEvent;
static int logReqFini( SrvrCommLogReqEvent* data, int status );

class SrvrCommLogReqEvent: public  CommLogReqEvent {
  public:
   	SrvrCommLogReqEvent( SerialBuf& buf ) : CommLogReqEvent( buf ) {  
		DBG("\n");
    }

    ~SrvrCommLogReqEvent( ) {
        DBG("\n");
    }

	bool process( EventGenerator* gen, EventChannel* ec ) {
        m_info = static_cast<Server*>(gen);

		DBGX("commID=%llu\n",commID);
		PWR_Obj obj = m_info->m_commMap[commID].objects[0];

        DBGX("obj='%s' attr=`%s`\n", PWR_ObjGetName(obj),
                            PWR_AttrGetTypeString( attrName ) );
        m_evChan = ec;
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
	EventChannel*		m_evChan;
};

static int logReqFini( SrvrCommLogReqEvent* data, int status )
{
	DBG("status=%d\n",status);

    data->m_respEvent.status = status;

    RouterEvent* ev = new RouterEvent( 0,0, &data->m_respEvent, true );

    data->m_evChan->sendEvent( ev );

    return 1;
}

}

#endif
