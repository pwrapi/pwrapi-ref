/* 
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#include <assert.h>
#include <unistd.h> //pause()

#include "eventChannel.h"
#include "debug.h"
#include "distRequest.h"
#include "communicator.h"
#include "distCntxt.h"
#include "event.h"
#include "events.h"
#include "distComm.h"

using namespace PowerAPI;

DistRequest::~DistRequest( ) {
}

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
		if ( ! ev ) {
			return PWR_RET_IPC;	
		}
		assert(ev);
		CommReq* req = (CommReq*)ev->id;
		req->process( ev );
		delete ev;
	}

	execCallback( );

    return PWR_RET_SUCCESS;
}

//* do we need to pass in req?, we are only using to delete the request 
// can we use "this"
void DistRequest::getSamples( DistCommReq* req, CommGetSamplesRespEvent* ev  )
{
	retval = ev->status;
	for ( unsigned i = 0; i< ev->count; i++ ) {
		((uint64_t*)value)[i] = ev->data[i];
	}
	*timeStamp = ev->startTime;
	DBGX("start time %lu, samples %d\n",*timeStamp, ev->count);
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

	for ( unsigned i = 0; i <  ev->value.size(); i++ ) {
		timeStamp[i] = ev->timeStamp[i];
		((uint64_t*)value)[i] = ev->value[i];
	}

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

	*status = PWR_RET_SUCCESS;
	while ( ! m_commReqs.empty() ) {	
		Event* ev = ec->getEvent();
		assert(ev);
		CommReq* req = (CommReq*)ev->id;
		req->process( ev );
		if ( ev->status != PWR_RET_SUCCESS ) {
			*status = ev->status;
			break;
		}
	}

	execCallback( );

    return PWR_RET_SUCCESS;
}
