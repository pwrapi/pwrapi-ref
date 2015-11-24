
#ifndef _EVENT_CHANNEL_H
#define _EVENT_CHANNEL_H

#include <assert.h>
#include <string>

struct Event;
struct SerialBuf;

class EventChannel {

  public:
	typedef Event* (*AllocFuncPtr)(unsigned int, SerialBuf& );

	EventChannel( AllocFuncPtr func, std::string name = "" ) : 
			m_allocFunc(func), m_name(name) {}
    virtual ~EventChannel() {}

	virtual EventChannel* accept() { assert(0); }
	virtual void close() { assert(0); } 

    virtual Event* getEvent( bool blocking = true ) = 0;
    virtual bool sendEvent( Event* ) = 0;
	virtual std::string& getName() { return m_name; }
  protected:
	AllocFuncPtr m_allocFunc;
	std::string m_name;
};

class ChannelSelect {
  public:

	struct Data {
		virtual ~Data() {}
	};

    virtual bool addChannel( EventChannel*, Data* ) = 0;
    virtual bool delChannel( EventChannel* ) = 0;
    virtual Data* wait() = 0;
};

EventChannel* getEventChannel( const std::string& type, 
		EventChannel::AllocFuncPtr, const std::string& config, const std::string& ); 

ChannelSelect* getChannelSelect( const std::string& type );

#endif
