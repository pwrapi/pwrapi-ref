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
