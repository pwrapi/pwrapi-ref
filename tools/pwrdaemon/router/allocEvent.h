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
