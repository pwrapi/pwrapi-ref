
#ifndef _DIST_COMM_H
#define _DIST_COMM_H

#include <set>
#include "communicator.h"

class EventChannel;

namespace PowerAPI {

class Object;
class DistCntxt;
class DistRequest;

class DistCommReq : public CommReq {
  public:
	DistCommReq( DistRequest* req ) : m_req( req ) {}
  protected:
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

	DistComm( DistCntxt*, std::set<Object*>& );
	~DistComm() {}
	virtual void getValue( PWR_AttrName, CommReq* req );
	virtual void setValue( PWR_AttrName, void* buf, CommReq* req );
	virtual void startLog( PWR_AttrName, CommReq* req );
	virtual void stopLog( PWR_AttrName, CommReq* req );
	virtual void getSamples( PWR_AttrName attr, PWR_Time*, double period,
			                unsigned int count, CommReq* req );

  private:
	static uint64_t m_currentCommID;

	EventChannel* 	m_ec;
	uint64_t 		m_commID;	
};

}

#endif
