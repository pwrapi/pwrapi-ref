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

#ifndef _EVENT_H
#define _EVENT_H

#include "serialize.h"
#include "eventType.h"

typedef uint64_t EventId;

class EventChannel;

class EventGenerator {
  public:
  	virtual ~EventGenerator() {} 
};

struct EventBase : public Serialize {

	virtual bool process( EventGenerator*, EventChannel* = NULL ) { return false; }
	virtual void serialize_out( SerialBuf& buf ) { }
	virtual void serialize_in( SerialBuf& buf ) { }
};

struct Event : public EventBase {

	Event( EventType _type = NotSet ) : type(_type), id(0), status(0) {}
	virtual ~Event() {}

    Event& operator=(const Event& other ) {
        type = other.type;
        id =   other.id;
        status = other.status;
        return *this;
    }

	//EventType	type;
    uint32_t    type;
    EventId   	id;
    int32_t    status;

	virtual void serialize_out( SerialBuf& buf ) {
		buf << id;
		buf << type;
		buf << status;
		EventBase::serialize_out( buf ); 
	}

	virtual void serialize_in( SerialBuf& buf ) {
		EventBase::serialize_in( buf ); 
		buf >> status;
		buf >> type;
		buf >> id;
	}
};

#endif
