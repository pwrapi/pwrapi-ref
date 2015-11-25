
#ifndef _ALLOC_EVENT_H
#define _ALLOC_EVENT_H

#include "event.h"
#include "serialize.h"

namespace PWR_Server {

Event* allocRtrEvent( unsigned int, SerialBuf& buf );
Event* allocReqEvent( unsigned int, SerialBuf& buf );

}

#endif
