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

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <assert.h>
#include <unistd.h> //pause()

#include "pwr.h"
#include "status.h"
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

int DistRequest::wait( )
{
	DistCntxt* ctx = static_cast<DistCntxt*>(m_cntxt);
	EventChannel* ec = ctx->getEventChannel();

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
	DBGX("count %"PRIu32"\n",ev->count);

#if 0
    // FIX ME 
    // need to pass up error codes somehome
	retval = ev->status;
#endif

	for ( unsigned i = 0; i< ev->count; i++ ) {
		((uint64_t*)value[0])[i] = ev->data[i];
	}
	DBGX("start time %"PRIu64", samples %"PRIu32"n",*timeStamp[0], ev->count);
	*timeStamp[0] = ev->startTime;
	m_commReqs.erase( req ); 
}

void DistRequest::setRetval( DistCommReq* req, CommLogRespEvent* ev )
{
	DBGX("\n");
	for ( unsigned i = 0; i < ev->errValue.size(); i++ ) {
		PWR_Obj obj;
		PWR_CntxtGetObjByName( m_cntxt, ev->errObj[i].c_str(), &obj ); 
		m_status->add( (Object*) obj, ev->errAttr[i], ev->errValue[i] );
	} 
	m_commReqs.erase( req ); 
}

void DistRequest::getValue( DistCommReq* req, CommRespEvent* ev )
{
	DBGX("\n");

	assert( value.size() == ev->value.size() );
	for ( unsigned i = 0; i < value.size(); i++ ) { 
		for ( unsigned j = 0; j <  ev->value[i].size(); j++ ) {
			((uint64_t*)value[i])[j] = ev->value[i][j];
			timeStamp[i][j] = ev->timeStamp[i][j];
		}
	}

	for ( unsigned i = 0; i < ev->errValue.size(); i++ ) {
		PWR_Obj obj;
		PWR_CntxtGetObjByName( m_cntxt, ev->errObj[i].c_str(), &obj ); 
		m_status->add( (Object*) obj, ev->errAttr[i], ev->errValue[i] );
	} 

	m_commReqs.erase( req ); 
}

void DistRequest::setValue( DistCommReq* req, CommRespEvent* ev )
{
	DBGX("\n");

	for ( unsigned i = 0; i < ev->errValue.size(); i++ ) {
		PWR_Obj obj;
		PWR_CntxtGetObjByName( m_cntxt, ev->errObj[i].c_str(), &obj ); 
		m_status->add( (Object*) obj, ev->errAttr[i], ev->errValue[i] );
	} 

	m_commReqs.erase( req ); 
}
