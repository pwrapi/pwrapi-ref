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
	struct ABC {
		EventChannel* send;
		EventChannel* recv;
	};
	enum { NEG_LINK, POS_LINK };
  public:
	TorusCore( RouterCoreArgs*, Router* ); 
	
	EventChannel* getChannel( RouterID id );
  private:
    std::vector< std::vector<ABC> >   m_rtrLinks;
};

}

#endif
