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

#ifndef _EVENTS_H
#define _EVENTS_H

#include <vector>
#include <string>
#include "pwrtypes.h"
#include "impTypes.h"
#include "event.h"

typedef std::string ObjID;
typedef uint64_t CommID;

struct ServerConnectEvent : public Event {
	ServerConnectEvent() : Event( ServerConnect ) {} 
	ServerConnectEvent( SerialBuf& buf ) {
		serialize_in(buf);
	}

	ObjID name; 
	virtual void serialize_out( SerialBuf& buf ) {
		Event::serialize_out(buf);
		buf << name;
	}

	virtual void serialize_in( SerialBuf& buf ) {
		buf >> name;
		Event::serialize_in(buf);
	}
};

struct CommEvent : public Event {
	CommEvent( EventType type ) : Event( type ), op( Noop ) {}
	CommEvent() { } 

    CommEvent& operator=( const CommEvent& other ) {
        commID = other.commID;
        op = other.op;
        return *this;
    }

    CommID          commID;

	enum OpType { Noop, Get, Set, Start, Stop, Clear } op;

	virtual void serialize_out( SerialBuf& buf ) {
		Event::serialize_out(buf);
		buf << commID;
		buf << op;
	}

	virtual void serialize_in( SerialBuf& buf ) {
		buf >> op;
		buf >> commID;
		Event::serialize_in(buf);
	}
};

struct CommCreateEvent : public CommEvent {
	CommCreateEvent( ) : CommEvent( CommCreate )  {}
	CommCreateEvent( SerialBuf& buf ) {
		serialize_in(buf);
	}

	CommCreateEvent(const CommCreateEvent& x) : members(x.members) {}

    std::vector< std::vector<ObjID > > members;

	virtual void serialize_in( SerialBuf& buf ) {
		buf >> members;
		CommEvent::serialize_in(buf);
	} 

	virtual void serialize_out( SerialBuf& buf ) {
		CommEvent::serialize_out(buf);
		buf << members;
	} 
};

struct CommDestroyEvent : public CommEvent {
	CommDestroyEvent( ) : CommEvent( CommDestroy )  {}
	CommDestroyEvent( SerialBuf& buf ) {
		serialize_in(buf);
	}

	virtual void serialize_in( SerialBuf& buf ) {
		CommEvent::serialize_in(buf);
	} 

	virtual void serialize_out( SerialBuf& buf ) {
		CommEvent::serialize_out(buf);
	} 
};

struct CommReqEvent : public CommEvent {
	CommReqEvent( ) : CommEvent( CommReq )  { }
	CommReqEvent( SerialBuf& buf ) {
		serialize_in(buf);
	}

	uint64_t grpIndex;	
    std::vector<PWR_AttrName> attrName;
    std::vector< uint64_t >   setValues;
	std::vector<ValueOp> valueOp;

	virtual void serialize_in( SerialBuf& buf ) {
		buf >> grpIndex;
		buf >> valueOp;
		buf >> attrName;
		buf >> setValues;
		CommEvent::serialize_in(buf);
	} 
	virtual void serialize_out( SerialBuf& buf ) {
		CommEvent::serialize_out(buf);
		buf << setValues;
		buf << attrName;
		buf << valueOp;
		buf << grpIndex;
	} 
};

struct CommRespEvent : public CommEvent {
	CommRespEvent( ) : CommEvent( CommResp )  { }
	CommRespEvent( SerialBuf& buf ) {
		serialize_in(buf);
	}

    std::vector< std::vector<PWR_Time> > timeStamp;
    std::vector< std::vector<uint64_t> > value;
	uint64_t grpIndex; 

	std::vector< ObjID >  		errObj;
	std::vector< PWR_AttrName > errAttr;
	std::vector< int >   		errValue;

	virtual void serialize_in( SerialBuf& buf ) {
		buf >> errObj;
		buf >> errAttr;
		buf >> errValue;

		buf >> grpIndex;
		buf >> value;
		buf >> timeStamp;
		CommEvent::serialize_in(buf);
	} 
	virtual void serialize_out( SerialBuf& buf ) {
		CommEvent::serialize_out(buf);
		buf << timeStamp;
		buf << value;
		buf << grpIndex;

		buf << errValue;
		buf << errAttr;
		buf << errObj;
	} 
};

struct CommLogReqEvent : public CommEvent {
	CommLogReqEvent( ) : CommEvent( CommLogReq )  { }
	CommLogReqEvent( SerialBuf& buf ) {
		serialize_in(buf);
	}
    PWR_AttrName attrName;

	virtual void serialize_in( SerialBuf& buf ) {
		buf >> attrName;
		CommEvent::serialize_in(buf);
	} 
	virtual void serialize_out( SerialBuf& buf ) {
		CommEvent::serialize_out(buf);
		buf << attrName;
	} 
};

struct CommLogRespEvent : public CommEvent {
	CommLogRespEvent( ) : CommEvent( CommLogResp )  { }
	CommLogRespEvent( SerialBuf& buf ) {
		serialize_in(buf);
	}

    CommLogRespEvent& operator=( const CommLogRespEvent& other ) {
        errObj = other.errObj;
        errAttr = other.errAttr;
        errValue = other.errValue;
        return *this;
    }

	std::vector< ObjID >  		errObj;
	std::vector< PWR_AttrName > errAttr;
	std::vector< int >   		errValue;

	virtual void serialize_in( SerialBuf& buf ) {
		buf >> errObj;
		buf >> errAttr;
		buf >> errValue;
		CommEvent::serialize_in(buf);
	} 
	virtual void serialize_out( SerialBuf& buf ) {
		CommEvent::serialize_out(buf);
		buf << errValue;
		buf << errAttr;
		buf << errObj;
	} 
};

struct CommGetSamplesReqEvent : public CommEvent {
	CommGetSamplesReqEvent( ) : CommEvent( CommGetSamplesReq )  { }
	CommGetSamplesReqEvent( SerialBuf& buf ) {
		serialize_in(buf);
	}
    PWR_AttrName attrName;
	PWR_Time startTime;
	double period;
	uint32_t count;

	virtual void serialize_in( SerialBuf& buf ) {
		buf >> attrName;
		buf >> startTime;
		buf >> period;
		buf >> count; 
		CommEvent::serialize_in(buf);
	} 
	virtual void serialize_out( SerialBuf& buf ) {
		CommEvent::serialize_out(buf);
		buf << count; 
		buf << period;
		buf << startTime;
		buf << attrName;
	} 
};

struct CommGetSamplesRespEvent : public CommEvent {
	CommGetSamplesRespEvent( ) : CommEvent( CommGetSamplesResp )  { }
	CommGetSamplesRespEvent( SerialBuf& buf ) {
		serialize_in(buf);
	}

    CommGetSamplesRespEvent& operator=( const CommGetSamplesRespEvent& other ) {
        errObj = other.errObj;
        errAttr = other.errAttr;
        errValue = other.errValue;
        startTime = other.startTime;
        count = other.count;
        data = other.data;
        return *this;
    }

	PWR_Time startTime;
	uint32_t count;
	std::vector< uint64_t > data;

	std::vector< ObjID >  		errObj;
	std::vector< PWR_AttrName > errAttr;
	std::vector< int32_t >   		errValue;

	virtual void serialize_in( SerialBuf& buf ) {
		buf >> startTime;
		buf >> count;
		buf >> data;
		buf >> errValue;
		buf >> errAttr;
		buf >> errObj;
		CommEvent::serialize_in(buf);
	} 
	virtual void serialize_out( SerialBuf& buf ) {
		CommEvent::serialize_out(buf);

		buf << errObj;
		buf << errAttr;
		buf << errValue;
		buf << data;
		buf << count;
		buf << startTime;

	} 
};

#endif

