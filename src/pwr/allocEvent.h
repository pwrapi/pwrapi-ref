
#ifndef _ALLOC_EVENT_H
#define _ALLOC_EVENT_H

#include "event.h"
#include "eventType.h"
#include "serialize.h"

Event* ctx_allocEvent( unsigned int, SerialBuf& buf );

#endif
