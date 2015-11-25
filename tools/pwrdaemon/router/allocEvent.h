
#ifndef _ALLOC_EVENT_H
#define _ALLOC_EVENT_H

#include "event.h"
#include "serialize.h"

namespace PWR_Router {

Event* allocClientEvent( unsigned int, SerialBuf& buf );
Event* allocServerEvent( unsigned int, SerialBuf& buf );
Event* allocRtrEvent( unsigned int, SerialBuf& buf );

}

#endif
