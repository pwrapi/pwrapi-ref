/* 
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#ifndef _PWR_NULLCONFIG_H
#define _PWR_NULLCONFIG_H

#include "pwrtypes.h"
#include "config.h"

namespace PowerAPI {

class NullConfig : public Config {
  public:
    NullConfig( std::string file );
	
    std::string findParent( std::string name );
    std::deque< std::string > findAttrChildren( std::string, PWR_AttrName );
    std::string findAttrOp( std::string, PWR_AttrName );
    std::string findObjLocation( std::string );
    std::deque< std::string > findChildren( std::string );
    std::deque< Config::ObjDev > findObjDevs( std::string, PWR_AttrName );
    std::deque< Config::Plugin > findPlugins();
    std::deque< Config::SysDev > findSysDevs();
    std::deque< std::string > findObjType( PWR_ObjType );
    Location findLocation( std::string );
    bool hasObject( const std::string name );
    PWR_ObjType objType( const std::string );
    void print( std::ostream& );
};

}

#endif
