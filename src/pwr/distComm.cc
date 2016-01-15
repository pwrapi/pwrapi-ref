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

DistComm::DistComm( DistCntxt* cntxt, std::set<Object*>& objects )
{
	DBGX("num objects %lu\n", objects.size() );
	m_ec = cntxt->getEventChannel();
	assert(m_ec);

	CommCreateEvent* ev = new CommCreateEvent();
	m_commID = ev->commID = ((CommID)gettid() << 32) | m_currentCommID++;

	std::set<Object*>::iterator iter = objects.begin();
	for ( ; iter != objects.end(); ++iter ) {
		ev->members.push_back((*iter)->name());
	}

	m_ec->sendEvent( ev );
	delete ev;
}

void DistComm::getValue( PWR_AttrName attr, ValueOp op, CommReq* req )
{
	DBGX("%s\n",attrNameToString(attr));

	CommReqEvent* ev = new CommReqEvent;	
	ev->commID = m_commID;
	ev->op = CommEvent::Get;
	ev->valueOp = op;
	ev->id = (EventId) req;	
	ev->attrName = attr; 
	m_ec->sendEvent( ev );
	delete ev;
}

void DistGetCommReq::process( Event* _ev ) {
	DBGX("\n");
	m_req->getValue( this, static_cast<CommRespEvent*>(_ev) );
}

void DistComm::setValue( PWR_AttrName attr, void* buf, CommReq* req )
{
	DBGX("%s\n",attrNameToString(attr));

	CommReqEvent* ev = new CommReqEvent;	
	ev->commID = m_commID;
	ev->op = CommEvent::Set;
	ev->id = (EventId) req;	
	ev->attrName = attr; 
	ev->value = *(uint64_t*)buf;
	m_ec->sendEvent( ev );
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
	m_ec->sendEvent( ev );
	delete ev;
}

void DistStartLogCommReq::process( Event* _ev ) {
	DBGX("\n");
	m_req->setStatus( this, static_cast<CommRespEvent*>(_ev) );
}

void DistStopLogCommReq::process( Event* _ev ) {
	DBGX("\n");
	m_req->setStatus( this, static_cast<CommRespEvent*>(_ev) );
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
	m_ec->sendEvent( ev );
	delete ev;
}

void DistComm::getSamples( PWR_AttrName attr, PWR_Time* time,
				double period, unsigned int count, CommReq* req )
{
	DBGX("%s period=%f count=%d\n",attrNameToString(attr), period, count );

	CommGetSamplesReqEvent* ev = new CommGetSamplesReqEvent;	
	ev->commID = m_commID;
	ev->id = (EventId) req;	
	ev->attrName = attr; 
	ev->startTime = *time;
	ev->period = period;
	ev->count = count;
	m_ec->sendEvent( ev );
	delete ev;
}
