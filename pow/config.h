
#ifndef _PWR_CONFIG_H
#define _PWR_CONFIG_H

#include <deque>
#include <string>
#include <iostream>

#include "types.h"

namespace PowerAPI {

class Config {
  public:

	virtual ~Config() {}
	struct SysDev {
		std::string name;
		std::string plugin;
		std::string initString;
	};

	struct ObjDev {
		std::string device;
		std::string openString;
	};

	struct Plugin {
		std::string name;
		std::string lib;
	};

    struct Location {
        std::string type;
        std::string config;
    };

    virtual std::string findParent( std::string ) = 0;
	virtual std::string findAttrOp( std::string, PWR_AttrName ) = 0;
	virtual std::deque< std::string > 
						findAttrChildren( std::string, PWR_AttrName ) = 0;
	virtual std::deque< std::string > findChildren( std::string ) = 0;
    virtual std::string findObjLocation( std::string ) = 0;
	virtual std::deque< ObjDev > findObjDevs( std::string, PWR_AttrName ) = 0;
	virtual std::deque< Plugin > findPlugins() = 0;
	virtual std::deque< SysDev > findSysDevs() = 0;
	virtual std::deque< std::string > findObjType( PWR_ObjType ) = 0; 
    virtual Location findLocation( std::string ) = 0;

	virtual bool hasObject( const std::string ) = 0;
	virtual PWR_ObjType objType( const std::string ) = 0;
	virtual void print( std::ostream& ) {};
};

}

#endif
