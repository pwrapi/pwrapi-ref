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

#include "routerSelect.h"
#include "router.h"
#include <event.h>
#include <debug.h>

using namespace PWR_Router;

bool EventData::process( ChannelSelect* sel, Router* rtr ) {
    Event* event = m_chan->getEvent();
    if ( NULL == event ) {
        DBGX("channel closed\n");
        sel->delChannel( m_chan );
        m_rtrChan->del( m_chan );
        delete m_chan;
		return true;
    } else {
        if ( event->process( static_cast<EventGenerator*>(rtr), m_chan ) ) {
			delete event;
		}
		return false;
    }
}

bool RouterData::process( ChannelSelect* sel, Router* rtr ) {
    Event* event = m_chan->getEvent();
    if ( NULL == event ) {
        DBGX("channel closed\n");
		m_chan->close();
		return false;
    } else {
        if ( event->process( static_cast<EventGenerator*>(rtr), m_chan ) ) {
			delete event;
		}
		return false;
    }
}
