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
	virtual void getValues(int, PWR_AttrName [], ValueOp [], CommReq* req ) = 0;
	virtual void setValues( int, PWR_AttrName [], void* buf, CommReq* req ) = 0;
	virtual void startLog( PWR_AttrName, CommReq* req ) = 0;
	virtual void stopLog( PWR_AttrName, CommReq* req ) = 0;
	virtual void getSamples( PWR_AttrName attr, PWR_Time start, double period, 
				unsigned int count, CommReq* req ) = 0;
};

}

#endif
