#ifndef _ROUTER_CORE_H
#define _ROUTER_CORE_H

#include <stdint.h>

class EventChannel;
class ChannelSelect;

namespace PWR_Router {

typedef uint32_t RouterID;

struct RouterCoreArgs {
	virtual ~RouterCoreArgs() {}
};

class RouterCore {
  public:
    virtual EventChannel* getChannel( RouterID ) = 0;
};

}

#endif
