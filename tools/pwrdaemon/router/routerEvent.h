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

#ifndef _ROUTER_EVENT_H
#define _ROUTER_EVENT_H

#include <event.h>

typedef uint64_t AppID;

struct RouterEvent : public Event {

	typedef Event* (*AllocFuncPtr)(unsigned int, SerialBuf& );

    RouterEvent( AppID _src, AppID _dest, Event* ev ) :
        Event( Router2Router ), src(_src), dest( _dest)
	{	
		ev->serialize_out( payload );
	    eventType = (EventType) ev->type;
	}

    RouterEvent( SerialBuf& buf ) {
        serialize_in(buf);
    }

	void initPayload( Event* ev ) {
		ev->serialize_out( payload );
		eventType = (EventType) ev->type;
	}

	Event* getPayload( AllocFuncPtr alloc ) {
		return alloc( eventType, payload ); 
	}

    AppID 	src;
    AppID 	dest;
	EventType	eventType;
	SerialBuf 	payload;

    virtual void serialize_out( SerialBuf& buf ) {
        Event::serialize_out(buf);
        buf << dest;
        buf << src;
		buf << eventType;
		buf << payload.buf;
    }

    virtual void serialize_in( SerialBuf& buf ) {
		buf >> payload.buf;
		buf >> eventType;
        buf >> src;
        buf >> dest;
        Event::serialize_in(buf);
    }
};

#endif
