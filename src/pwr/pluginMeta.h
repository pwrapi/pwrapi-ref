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

#include <dlfcn.h>
#include <map>
#include <set>
//#include <deque>
#include <vector>
#include "util.h"
#include "pwrdev.h"

#ifndef _PLUGINMETA_H
#define _PLUGINMETA_H

class PluginMeta {
	struct Object {
	};
  public:
	PluginMeta( std::string libName ) {

    	void* m_lib = dlopen( libName.c_str(), RTLD_LAZY);
		assert(m_lib);

    	getMetaFuncPtr_t func =   (getMetaFuncPtr_t)dlsym(m_lib, GETMETAFUNC );
    	assert(func);

    	m_meta = (*func)();

    	//printf("%d\n",meta->numObjs());
	    //std::vector< PWR_ObjType > objs( meta->numObjs() );

		m_objVec.resize( m_meta->numObjs() );
   		m_meta->readObjs( m_objVec.size(), &m_objVec[0] );
    	for ( unsigned i = 0; i < m_objVec.size(); i++ ) {
			std::vector< PWR_AttrName > attrs( m_meta->numAttrs(m_objVec[i]) );
        	//printf("%s\n", objTypeToString( m_objVec[i] ) );
			
        	m_meta->readAttrs( attrs.size(), &attrs[0]);
        	for ( unsigned j = 0; j < attrs.size(); j++ ) {
            	//printf("add %s %s\n", objTypeToString( m_objVec[i] ), 
				//			attrNameToString(attrs[j]));
				m_objMap[ m_objVec[i] ].insert( attrs[j] ); 
        	}
		}
	}
    ~PluginMeta() {
        dlclose( m_lib );
    }

	bool findAttr( PWR_ObjType type, PWR_AttrName attr ) {
		std::string retval;	
		if ( m_objMap.find( type ) != m_objMap.end() ) {
			if ( m_objMap[type].find( attr ) != m_objMap[type].end() ) {
				return true;
			}
		}
		return false;
	}

    std::string getPluginName( ) {
        std::string ret(100,' '); 
        m_meta->getPluginName( 100, &ret[0] ); 
        return ret; 
    }
	
    std::string getDevInitStr( std::string devName ) {
        std::string ret(100,' '); 
        m_meta->getDevInitStr( &devName[0], 100, &ret[0] );  
        return ret; 
    }

    std::string getDevName( PWR_ObjType type ) {
        std::string ret(100,' '); 
        m_meta->getDevName( type, 100, &ret[0] ); 
        return ret; 
    }

    std::string getDevOpenStr( PWR_ObjType type, int global_index  ) {
        std::string ret(100,' '); 
        m_meta->getDevOpenStr( type, global_index, 100, &ret[0] );
        return ret; 
    }

    bool isObjectSupported( PWR_ObjType obj, PWR_AttrName attr ) {
        return m_objMap.find(obj) != m_objMap.end() &&
				m_objMap[obj].find(attr) != m_objMap[obj].end();
    }
    

  private:
    void*           m_lib;
   	plugin_meta_t*  m_meta;
    std::map< PWR_ObjType,std::set<PWR_AttrName> > m_objMap;
	std::vector< PWR_ObjType > 					   m_objVec;
};

#endif
