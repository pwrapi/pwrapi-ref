
#ifndef _ROUTER_EVENT_H
#define _ROUTER_EVENT_H

#include <event.h>

typedef uint64_t AppID;

struct RouterEvent : public Event {

	typedef Event* (*AllocFuncPtr)(unsigned int, SerialBuf& );

    RouterEvent( AppID _src, AppID _dest, Event* ev, bool resp = false ) :
        Event( Router2Router ), src(_src), dest( _dest),
		isResponse( resp )
	{	
		ev->serialize_out( payload );
	    eventType = ev->type;
	}

    RouterEvent( SerialBuf& buf ) {
        serialize_in(buf);
    }

	void initPayload( Event* ev ) {
		ev->serialize_out( payload );
		eventType = ev->type;
	}

	Event* getPayload( AllocFuncPtr alloc ) {
		return alloc( eventType, payload ); 
	}

    AppID 	src;
    AppID 	dest;
	EventType	eventType;
	SerialBuf 	payload;
	bool      	isResponse;

    virtual void serialize_out( SerialBuf& buf ) {
        Event::serialize_out(buf);
        buf << dest;
		buf << eventType;
		buf << payload.buf;
		buf << isResponse;
    }

    virtual void serialize_in( SerialBuf& buf ) {
		buf >> isResponse;
		buf >> payload.buf;
		buf >> eventType;
        buf >> dest;
        Event::serialize_in(buf);
    }
};

#endif
