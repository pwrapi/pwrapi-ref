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

#include "nullConfig.h"

#include <deque>

using namespace PowerAPI;

NullConfig::NullConfig( std::string file )
{
}
	
std::string NullConfig::findParent( std::string name )
{
    return std::string();
}

std::deque< std::string > NullConfig::findAttrChildren( std::string, PWR_AttrName )
{
    return std::deque< std::string >();
}

std::string NullConfig::findAttrOp( std::string, PWR_AttrName )
{
    return std::string();
}

std::string NullConfig::findObjLocation( std::string )
{
    return std::string();
}

std::deque< std::string > NullConfig::findChildren( std::string )
{
    return std::deque< std::string >();
}

std::deque< Config::ObjDev > NullConfig::findObjDevs( std::string, PWR_AttrName )
{
    return std::deque< Config::ObjDev >();
}

std::deque< Config::Plugin > NullConfig::findPlugins()
{
    return std::deque< Config::Plugin >();
}

std::deque< Config::SysDev > NullConfig::findSysDevs()
{
    return std::deque< Config::SysDev >();
}

std::deque< std::string > NullConfig::findObjType( PWR_ObjType )
{
    return std::deque< std::string >();
}

Config::Location NullConfig::findLocation( std::string )
{
    return Config::Location();
}

bool NullConfig::hasObject( const std::string name )
{
    return false;
}

PWR_ObjType NullConfig::objType( const std::string )
{
    return PWR_OBJ_INVALID;
}

void NullConfig::print( std::ostream& )
{
}


