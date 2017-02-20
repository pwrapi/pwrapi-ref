
#ifndef _SRVR_COMM_REQ_EVENT_H
#define _SRVR_COMM_REQ_EVENT_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <events.h>
#include <debug.h>
#include "server.h"

namespace PWR_Server {

static void requestFini( void* );

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

		DBGX("commID=%"PRIx64" grpIndex=%"PRIu64"\n",commID, grpIndex);
		char name[100];
		PWR_ObjGetName(obj,name,100);

   		DBGX("obj='%s':\n", name );
		for ( unsigned i = 0; i < attrName.size(); i++ ) {
    		DBGX("    attr=`%s`\n", PWR_AttrGetTypeString( attrName[i] ) );
		}
	
		if ( op == Get ) {
			m_respEvent.value.resize(1);
			m_respEvent.timeStamp.resize(1);
			m_respEvent.value[0].resize(attrName.size());
			m_respEvent.timeStamp[0].resize(attrName.size());
		}

		m_respEvent.op = op;
    	m_respEvent.id = id;
		m_respEvent.grpIndex = grpIndex;
		// quiet valgrind
		m_respEvent.commID = 0;

		int rc = PWR_StatusCreate(m_info->m_ctx,&m_status);
		assert( rc == PWR_RET_SUCCESS );
    	m_req = PWR_ReqCreateCallback( m_info->m_ctx, m_status,
										requestFini, this );
    	assert( m_req );

    	int ret;
    	if ( op == CommEvent::Get ) {
        	ret = PWR_ObjAttrGetValues_NB( obj, attrName.size(), 
				&attrName[0], &m_respEvent.value[0][0],
				&m_respEvent.timeStamp[0][0], m_req );
    	} else {
        	ret = PWR_ObjAttrSetValues_NB( obj, attrName.size(),
				&attrName[0], &setValues[0], m_req );
    	}

    	if ( ret != PWR_RET_SUCCESS ) {
        	requestFini( this );
    	}

		return false;
	}

	Server*			m_info;
    CommRespEvent   m_respEvent;
	PWR_Request	    m_req;
	PWR_Status		m_status;
};

static void requestFini( void* _data )
{
	SrvrCommReqEvent* data = (SrvrCommReqEvent*) _data;
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
