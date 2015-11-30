
#ifndef _TORUS_CORE_H
#define _TORUS_CORE_H

#include <map>
#include <vector>
#include <string>
#include "routerCore.h"
#include <debug.h>

namespace PWR_Router {

class Router;

struct Dim {
   	std::string posPort;
    std::string negSrvr;
    std::string negSrvrPort;
};

struct TorusArgs : public RouterCoreArgs {
	std::vector< Dim > dim;
};

class TorusCore : public RouterCore {
	enum { NEG_LINK, POS_LINK };
  public:
	TorusCore( RouterCoreArgs*, Router* ); 
	
	EventChannel* getChannel( RouterID id ) {
		DBGX("\n");
		return m_rtrLinks[0][NEG_LINK];
	}
  private:
    std::vector< std::vector<EventChannel*> >   m_rtrLinks;
};

}

#endif
