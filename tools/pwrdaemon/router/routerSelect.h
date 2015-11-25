
#ifndef _ROUTER_SELECT_H
#define _ROUTER_SELECT_H

#include "eventChannel.h"

namespace PWR_Router {

class Router;

class ChanBase {
  public:
	virtual ~ChanBase() {}
	virtual void add( EventChannel* ) = 0;
	virtual void del( EventChannel* ) = 0;
};

class SelectData : public ChannelSelect::Data {
  public:
	SelectData( EventChannel* chan, ChanBase* rtrChan ) :
		m_chan( chan ), m_rtrChan( rtrChan )
	{ }
	virtual bool process( ChannelSelect*, Router* ) = 0;

  protected:
	EventChannel* m_chan;
	ChanBase*	  m_rtrChan;
};

template <class T>
class AcceptData : public SelectData {
  public:
    AcceptData( EventChannel* chan, ChanBase* rtrChan ) :
        SelectData( chan, rtrChan )
    {}

    bool process( ChannelSelect* sel, Router* rtr ) {
        EventChannel* newChan = m_chan->accept();
        sel->addChannel( newChan, new T( newChan, m_rtrChan ) );
		m_rtrChan->add( newChan );
        return false;
    }
};

class EventData : public SelectData {
  public:
    EventData( EventChannel* chan, ChanBase* rtrChan ) :
        SelectData( chan, rtrChan )
	{}

    bool process( ChannelSelect* sel, Router* rtr );
};

class RouterData : public SelectData {
  public:
    RouterData( EventChannel* chan, ChanBase* rtrChan ) :
        SelectData( chan, rtrChan )
	{}

    bool process( ChannelSelect* sel, Router* rtr );
};

}

#endif
