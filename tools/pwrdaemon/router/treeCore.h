
#ifndef _TREE_CORE_H
#define _TREE_CORE_H

#include <map>
#include <vector>
#include <string>
#include "routerCore.h"
#include <debug.h>

namespace PWR_Router {

class Router;

struct Link {
	std::string myListenPort;
	std::string otherHost;
	std::string otherHostListenPort;
	std::string nidStart;
	std::string nidStop;
};

struct TreeArgs : public RouterCoreArgs {
	std::vector<Link> links;
};

class TreeCore : public RouterCore {
   	struct ABC {
		ABC() : send(NULL), recv(NULL), nidStart(0), nidStop(0) {}
        EventChannel* send;
        EventChannel* recv;
		uint32_t nidStart;
		uint32_t nidStop;
    };

  public:
	TreeCore( RouterCoreArgs*, Router* ); 
	
	EventChannel* getChannel( RouterID id );

  private:
    std::vector< ABC  >   m_rtrLinks;
};

}

#endif
