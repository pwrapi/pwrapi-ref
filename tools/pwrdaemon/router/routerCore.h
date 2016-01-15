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

#ifndef _ROUTER_CORE_H
#define _ROUTER_CORE_H

#include <stdint.h>

class EventChannel;
class ChannelSelect;

namespace PWR_Router {

typedef uint32_t RouterID;

struct RouterCoreArgs {
	virtual ~RouterCoreArgs() {}
	std::string type;
};

class RouterCore {
  public:
    virtual EventChannel* getChannel( RouterID ) = 0;
};

}

#endif
