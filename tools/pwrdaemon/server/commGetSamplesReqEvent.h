
#ifndef _SRVR_COMM_GET_SAMPLES_REQ_EVENT_H
#define _SRVR_COMM_GET_SAMPLES_REQ_EVENT_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <events.h>
#include <eventChannel.h>
#include <debug.h>
#include "server.h"


namespace PWR_Server {

static void getSamplesFini( void* );

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

		DBGX("commID=%" PRIx64 "\n",commID);

		char name[100];
		PWR_ObjGetName(obj,name,100);
    	DBGX("obj='%s' attr=`%s`\n", name,
                            PWR_AttrGetTypeString( attrName ) );
		DBGX("period=%f count=%d\n", period, count );
    	m_respEvent.id = id;
		m_respEvent.data.resize( count );
		m_respEvent.count = count;
		
		PWR_StatusCreate(m_info->m_ctx,&m_status);
    	m_req = PWR_ReqCreateCallback( m_info->m_ctx, m_status, 
											(Callback)getSamplesFini, this );
    	assert( m_req );
			
    	int ret = PWR_ObjAttrGetSamples_NB( obj, attrName,
           		&m_respEvent.startTime, period, &m_respEvent.count, 
				(void*) &m_respEvent.data[0], m_req );

    	if ( ret != PWR_RET_SUCCESS ) {
        	getSamplesFini( this );
    	}

		return false;
	}

    CommGetSamplesRespEvent   m_respEvent;

	Server*			m_info;
	PWR_Request	    m_req;
	PWR_Status		m_status;
};

static void getSamplesFini( void* _data )
{
	SrvrCommGetSamplesReqEvent* data = (SrvrCommGetSamplesReqEvent*) _data; 
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
