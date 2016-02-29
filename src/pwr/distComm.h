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

#ifndef _DIST_COMM_H
#define _DIST_COMM_H

#include <vector>
#include <set>
#include "communicator.h"
#include "events.h"

class EventChannel;

namespace PowerAPI {

class Object;
class DistCntxt;
class DistRequest;

class DistCommReq : public CommReq {
  public:
	DistCommReq( DistRequest* req ) : m_req( req ) {}
	DistRequest* m_req;
};

class DistSetCommReq : public DistCommReq {
  public:
	DistSetCommReq( DistRequest* req ) : DistCommReq(req ) {}
	void process( Event* );
};

class DistGetCommReq : public DistCommReq {
  public:
	DistGetCommReq( DistRequest* req ) : DistCommReq(req ) {}
	uint64_t buf;
	PWR_Time timeStamp;
	void process( Event* ); 
};

class DistStartLogCommReq : public DistCommReq {
  public:
	DistStartLogCommReq( DistRequest* req ) : DistCommReq(req ) {}
	void process( Event* ); 
};

class DistStopLogCommReq : public DistCommReq {
  public:
	DistStopLogCommReq( DistRequest* req ) : DistCommReq(req ) {}
	void process( Event* ); 
};

class DistGetSamplesCommReq : public DistCommReq {
  public:
	DistGetSamplesCommReq( DistRequest* req ) : DistCommReq(req ) {}
	void process( Event* ); 
};


class DistComm : public Communicator {

  public:

	DistComm( DistCntxt*, std::set<std::string>& );
	DistComm( DistCntxt* );
	~DistComm() {}
	std::vector<std::string>& getObjects() { return m_objects; }
	virtual void getValues( int, PWR_AttrName [], ValueOp [], CommReq* req );
	virtual void setValues( int, PWR_AttrName [], void* values, CommReq* req );
	virtual void startLog( PWR_AttrName, CommReq* req );
	virtual void stopLog( PWR_AttrName, CommReq* req );
	virtual void getSamples( PWR_AttrName attr, PWR_Time start,
						double period, unsigned int count, CommReq* req );

  private:
	EventChannel& getChannel();
	std::vector<std::string> m_objects;

  protected:
	static uint32_t m_currentCommID;

	DistCntxt*		m_ctx;
	EventChannel* 	m_ec;
	CommID 			m_commID;	
};

}

#endif
