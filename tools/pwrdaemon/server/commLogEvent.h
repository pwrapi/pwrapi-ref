
#ifndef _SRVR_COMM_LOG_EVENT_H
#define _SRVR_COMM_LOG_EVENT_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <events.h>
#include <debug.h> 
#include "server.h"

namespace PWR_Server {

static void logReqFini( void* );

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

		DBGX("commID=%" PRIx64 "\n",commID);
		PWR_Obj obj = m_info->m_commMap[commID].objects[0];

		char name[100];
		PWR_ObjGetName(obj,name,100);
		DBGX("obj='%s' attr=`%s`\n", name, PWR_AttrGetTypeString( attrName ) );
        m_respEvent.op = op;
        m_respEvent.id = id;

        m_respEvent.commID = 0;

        PWR_StatusCreate(m_info->m_ctx,&m_status);
        m_req = PWR_ReqCreateCallback( m_info->m_ctx, m_status,
                                            (Callback)logReqFini, this );
        assert( m_req );

        int ret;
        if ( op == CommEvent::Start ) {
            ret = PWR_ObjAttrStartLog_NB( obj, attrName, m_req );
        } else {
            ret = PWR_ObjAttrStopLog_NB( obj, attrName, m_req );
        }

        if ( ret != PWR_RET_SUCCESS ) {
            logReqFini( this );
        }

		return false;
	}

	PWR_Request			m_req;
	Server* 			m_info;
	CommLogRespEvent 	m_respEvent;
	PWR_Status			m_status;
};

static void logReqFini( void* _data )
{
	SrvrCommLogReqEvent* data = (SrvrCommLogReqEvent*) _data;
	DBG4("PWR_Server","\n");

    PWR_Status status = data->m_status;
    PWR_AttrAccessError error;

    while ( PWR_RET_EMPTY != PWR_StatusPopError( status, &error ) ) {
        data->m_respEvent.errValue.push_back( error.error ) ;
        data->m_respEvent.errAttr.push_back( error.name ) ;
        char name[100];
        PWR_ObjGetName( error.obj, name, 100 );
        data->m_respEvent.errObj.push_back( name );
    }

	PWR_StatusDestroy( data->m_status );

	data->m_info->fini( data, &data->m_respEvent );
}

}

#endif
