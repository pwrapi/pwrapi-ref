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

#ifndef _RTR_RTR_ROUTER_EVENT_H
#define _RTR_RTR_ROUTER_EVENT_H

#include <eventChannel.h>
#include <debug.h>
#include "routerEvent.h"
#include "router.h"

namespace PWR_Router {

class RtrRouterEvent: public  RouterEvent {
  public:
    RtrRouterEvent( SerialBuf& buf ) : RouterEvent( buf ) {}

    bool process( EventGenerator* _rtr, EventChannel* ec ) {
        Router& rtr = *static_cast<Router*>(_rtr);
		DBGX("dest=%" PRIx64 "\n",dest);
		rtr.sendEvent( dest, this );

        return true;
    }
};

}

#endif
