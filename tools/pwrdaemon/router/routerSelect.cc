#include "routerSelect.h"
#include "router.h"
#include <event.h>

using namespace PWR_Router;

bool EventData::process( ChannelSelect* sel, Router* rtr ) {
    Event* event = m_chan->getEvent();
    if ( NULL == event ) {
        printf("channel closed\n");
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
        printf("channel closed\n");
        sel->delChannel( m_chan );
		m_chan->close();
		return true;
    } else {
        if ( event->process( static_cast<EventGenerator*>(rtr), m_chan ) ) {
			delete event;
		}
		return false;
    }
}
