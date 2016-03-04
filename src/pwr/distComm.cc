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

#include <sys/syscall.h>
#include "util.h"
#include "distComm.h"
#include "distCntxt.h"
#include "distRequest.h"
#include "object.h"
#include "debug.h"
#include "events.h"
#include "eventChannel.h"

using namespace PowerAPI;

static inline int gettid() {
	return syscall(SYS_gettid);
}

uint32_t DistComm::m_currentCommID = 1;

DistComm::DistComm( DistCntxt* cntxt ) :
	m_ctx( cntxt), m_ec(NULL)
{
	m_commID = ((CommID)gettid() << 32) | m_currentCommID++;
}

DistComm::DistComm( DistCntxt* cntxt, std::set<std::string>& objects ) :
	m_ctx( cntxt), m_ec(NULL)
{
	std::set<std::string>::iterator iter = objects.begin();
	for ( ; iter != objects.end(); ++iter ) {
		m_objects.push_back( *iter );
	}
	DBGX("num objects %lu\n", objects.size() );
	m_commID = ((CommID)gettid() << 32) | m_currentCommID++;
}

EventChannel& DistComm::getChannel()
{
	if ( m_ec ) return *m_ec;

	m_ec = m_ctx->getEventChannel();
	assert(m_ec);

	CommCreateEvent* ev = new CommCreateEvent();
	ev->commID = m_commID;

	ev->members.push_back( m_objects );

	m_ec->sendEvent( ev );
	delete ev;
	return *m_ec;
}

void DistComm::getValues( int count, PWR_AttrName attr[],
										ValueOp op[], CommReq* req )
{
	CommReqEvent* ev = new CommReqEvent;	
	ev->commID = m_commID;
	ev->op = CommEvent::Get;
	for ( int i = 0; i < count; i++ ) {
		DBGX("%s\n",attrNameToString(attr[i]));
		ev->valueOp.push_back( op[i] );
		ev->attrName.push_back( attr[i] ); 
	}
	// don't need to set this but it gets rid of a valgrind error
	// should there be a different get and set events?
	ev->grpIndex = 0;
	ev->id = (EventId) req;	
	getChannel().sendEvent( ev );
	delete ev;
}

void DistGetCommReq::process( Event* _ev ) {
	DBGX("\n");
	m_req->getValue( this, static_cast<CommRespEvent*>(_ev) );
}

void DistComm::setValues( int count, PWR_AttrName attr[], 
						void* values, CommReq* req )
{

	CommReqEvent* ev = new CommReqEvent;	
	ev->commID = m_commID;
	ev->op = CommEvent::Set;
	ev->id = (EventId) req;	
	for ( int i = 0; i < count; i++ ) {
		DBGX("%s\n",attrNameToString(attr[i]));
		ev->attrName.push_back( attr[i] ); 
		ev->setValues.push_back( ((uint64_t*)values)[i] );
	}
	getChannel().sendEvent( ev );
	delete ev;
}

void DistSetCommReq::process( Event* _ev ) {
	DBGX("\n");
	m_req->setValue( this, static_cast<CommRespEvent*>(_ev) );
}

void DistComm::startLog( PWR_AttrName attr, CommReq* req )
{
	DBGX("%s\n",attrNameToString(attr));

	CommLogReqEvent* ev = new CommLogReqEvent;	
	ev->commID = m_commID;
	ev->op = CommEvent::Start;
	ev->id = (EventId) req;	
	ev->attrName = attr; 
	getChannel().sendEvent( ev );
	delete ev;
}

void DistStartLogCommReq::process( Event* _ev ) {
	DBGX("\n");
	m_req->setRetval( this, static_cast<CommLogRespEvent*>(_ev) );
}

void DistStopLogCommReq::process( Event* _ev ) {
	DBGX("\n");
	m_req->setRetval( this, static_cast<CommLogRespEvent*>(_ev) );
}

void DistGetSamplesCommReq::process( Event* _ev ) {
	DBGX("\n");
	m_req->getSamples( this, static_cast<CommGetSamplesRespEvent*>(_ev) );
}

void DistComm::stopLog( PWR_AttrName attr, CommReq* req )
{
	DBGX("%s\n",attrNameToString(attr));

	CommLogReqEvent* ev = new CommLogReqEvent;	
	ev->commID = m_commID;
	ev->op = CommEvent::Stop;
	ev->id = (EventId) req;	
	ev->attrName = attr; 
	getChannel().sendEvent( ev );
	delete ev;
}

void DistComm::getSamples( PWR_AttrName attr, PWR_Time start,
				double period, unsigned int count, CommReq* req )
{
	DBGX("%s period=%f count=%d\n",attrNameToString(attr), period, count );

	CommGetSamplesReqEvent* ev = new CommGetSamplesReqEvent;	
	ev->commID = m_commID;
	ev->id = (EventId) req;	
	ev->attrName = attr; 
	ev->startTime = start;
	ev->period = period;
	ev->count = count;
	getChannel().sendEvent( ev );
	delete ev;
}
