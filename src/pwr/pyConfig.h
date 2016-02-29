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

#ifndef _PWR_PYCONFIG_H
#define _PWR_PYCONFIG_H

#include <Python.h>

#include <pthread.h>
#include "pwrtypes.h"
#include "config.h"

namespace PowerAPI {

class PyConfig : public Config {
  public:
	PyConfig( std::string file );
	~PyConfig();
	
    std::string findParent( std::string name );
	std::deque< std::string >
                findAttrChildren( std::string, PWR_AttrName );
	std::string findAttrOp( std::string, PWR_AttrName );
	std::string findAttrHz( std::string, PWR_AttrName );
	std::string findAttrType( std::string, PWR_AttrName );
	std::deque< std::string > findChildren( std::string );
	std::deque< Config::ObjDev > findObjDevs( std::string, PWR_AttrName );
	std::deque< Config::Plugin > findPlugins();
	std::deque< Config::SysDev > findSysDevs();
	bool hasServer( std::string );
	bool hasObject( const std::string name );
	PWR_ObjType objType( const std::string );
	void print( std::ostream& );

  private:
    std::string findObjLocation( std::string );
    std::string attrNameToString( PWR_AttrName );
    PWR_ObjType objTypeStrToInt( const std::string );
    std::string objTypeToString( PWR_ObjType type );

	void lock() {
		pthread_mutex_lock(&m_mutex);
		//printf("locked %p\n", pthread_self());
	}

	void unlock() {
		pthread_mutex_unlock(&m_mutex);
		//printf("unlocked %p\n",pthread_self());
	}

    static pthread_mutex_t	m_mutex;
    static PyObject* m_pModule;
};

}

#endif
