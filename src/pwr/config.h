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

#ifndef _PWR_CONFIG_H
#define _PWR_CONFIG_H

#include <deque>
#include <string>
#include <iostream>

#include "pwrtypes.h"

namespace PowerAPI {

class Config {
  public:

	virtual ~Config() {}
	struct SysDev {
		std::string name;
		std::string plugin; // this points to a Plugin.name
		std::string initString;
	};

	struct ObjDev {
		std::string device; // this points to a SysDev.name
		std::string openString;
	};

	struct Plugin {
		std::string name;
		std::string lib;
	};

    virtual std::string findParent( std::string ) = 0;
	virtual std::string findAttrHz( std::string, PWR_AttrName ) = 0;
	virtual std::string findAttrOp( std::string, PWR_AttrName ) = 0;
	virtual std::string findAttrType( std::string, PWR_AttrName ) = 0;
	virtual std::deque< std::string > 
						findAttrChildren( std::string, PWR_AttrName ) = 0;

	virtual std::deque< std::string > findChildren( std::string ) = 0;
	virtual bool hasServer( std::string ) = 0;
	virtual std::deque< ObjDev > findObjDevs( std::string, PWR_AttrName ) = 0;
	virtual std::deque< Plugin > findPlugins() = 0;
	virtual std::deque< SysDev > findSysDevs() = 0;

	virtual bool hasObject( const std::string ) = 0;
	virtual PWR_ObjType objType( const std::string ) = 0;
	virtual void print( std::ostream& ) {};
};

}

#endif
