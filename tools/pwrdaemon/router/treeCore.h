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
		unsigned nidStart;
		unsigned nidStop;
    };

  public:
	TreeCore( RouterCoreArgs*, Router* ); 
	
	EventChannel* getChannel( RouterID id );

  private:
    std::vector< ABC  >   m_rtrLinks;
};

}

#endif
