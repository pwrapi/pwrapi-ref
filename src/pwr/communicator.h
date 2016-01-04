
#ifndef _COMMUNICATOR_H
#define _COMMUNICATOR_H

#include "pwrtypes.h"
#include "impTypes.h"

struct Event;

namespace PowerAPI {

class CommReq {
  public:
	  virtual ~CommReq() {} 
	  virtual void process( Event* ) = 0;
};

class Communicator {
  public:
	Communicator() {}
	virtual ~Communicator() {}
	virtual void getValue( PWR_AttrName, ValueOp, CommReq* req ) = 0;
	virtual void setValue( PWR_AttrName, void* buf, CommReq* req ) = 0;
	virtual void startLog( PWR_AttrName, CommReq* req ) = 0;
	virtual void stopLog( PWR_AttrName, CommReq* req ) = 0;
	virtual void getSamples( PWR_AttrName attr, PWR_Time*, double period, 
				unsigned int count, CommReq* req ) = 0;
};

}

#endif
