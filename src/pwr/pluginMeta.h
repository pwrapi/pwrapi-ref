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

    	void* ptr = dlopen( libName.c_str(), RTLD_LAZY);
		assert(ptr);

    	getMetaFuncPtr_t func =   (getMetaFuncPtr_t)dlsym(ptr, GETMETAFUNC );
    	assert(func);

    	plugin_meta_t* meta = (*func)();

    	//printf("%d\n",meta->numObjs());
	//std::vector< PWR_ObjType > objs( meta->numObjs() );

		m_objVec.resize( meta->numObjs() );
   		meta->readObjs( m_objVec.size(), &m_objVec[0] );
    	for ( unsigned i = 0; i < m_objVec.size(); i++ ) {
			std::vector< PWR_AttrName > attrs( meta->numAttrs(m_objVec[i]) );
        	//printf("%s\n", objTypeToString( objs[i] ) );
			
        	meta->readAttrs( attrs.size(), &attrs[0]);
        	for ( unsigned j = 0; j < attrs.size(); j++ ) {
            	//printf("\t%s\n",attrNameToString(attrs[j]));
				m_objMap[ m_objVec[i] ].insert( attrs[j] ); 
        	}
		}
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
	
	std::vector<PWR_ObjType>& getSupportedObjects() {
		return m_objVec;
	}

  private:
    std::map< PWR_ObjType,std::set<PWR_AttrName> > m_objMap;
	std::vector< PWR_ObjType > 					   m_objVec;
};

#endif
