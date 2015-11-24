#include <assert.h>
#include <unistd.h> //pause()

#include "eventChannel.h"
#include "debug.h"
#include "distRequest.h"
#include "communicator.h"
#include "distCntxt.h"
#include "event.h"
#include "events.h"
//#include "distComm.h"

using namespace PowerAPI;

int DistRequest::check( int* status )
{
    if ( m_commReqs.empty() ) {
		execCallback( );
		if ( status ) {
			*status = retval;
		}
	}	
    return m_commReqs.empty() ? PWR_RET_SUCCESS : PWR_RET_FAILURE;
}

int DistRequest::wait( Status* status )
{
	DistCntxt* ctx = static_cast<DistCntxt*>(m_cntxt);
	EventChannel* ec = ctx->getEventChannel();
	DBGX("%lu\n", m_commReqs.size());

	while ( ! m_commReqs.empty() ) {	
		Event* ev = ec->getEvent();
		assert(ev);
		CommReq* req = (CommReq*)ev->id;
		req->process( ev );
	}

	execCallback( );

    return PWR_RET_SUCCESS;
}

//* do we need to pass in req?, we are only using to delete the request 
// can we use "this"
void DistRequest::getSamples( DistCommReq* req, CommGetSamplesRespEvent* ev  )
{
	CommGetSamplesRespEvent* event= static_cast<CommGetSamplesRespEvent*>(ev);		
	retval = ev->status;
	for ( int i = 0; i< ev->count; i++ ) {
		((uint64_t*)value)[i] = ev->data[i];
	}
	*timeStamp = ev->startTime;
	DBGX("start time %llu, samples %d\n",*timeStamp, ev->count);
	m_commReqs.erase( req ); 
}

void DistRequest::setStatus( DistCommReq* req, CommRespEvent* ev  )
{
	DBGX("\n");
	retval = ev->status;
	m_commReqs.erase( req ); 
}

void DistRequest::getValue( DistCommReq* req, CommRespEvent* ev )
{
	retval = ev->status;
	*timeStamp = ev->timeStamp;
	*(uint64_t*)value = ev->value;

	m_commReqs.erase( req ); 
}

void DistRequest::setValue( DistCommReq* req, CommRespEvent* ev )
{
	DBGX("\n");
	retval = ev->status;
	m_commReqs.erase( req ); 
}

int DistRequest::wait( int* status )
{
	DistCntxt* ctx = static_cast<DistCntxt*>(m_cntxt);
	EventChannel* ec = ctx->getEventChannel();
	DBGX("%lu\n", m_commReqs.size());

	while ( ! m_commReqs.empty() ) {	
		Event* ev = ec->getEvent();
		assert(ev);
		CommReq* req = (CommReq*)ev->id;
		req->process( ev );
		*status = ev->status;
	}

	execCallback( );

    return PWR_RET_SUCCESS;
}
