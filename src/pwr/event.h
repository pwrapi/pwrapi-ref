
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

	EventType	type;
    EventId   	id;
    int         status;

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
