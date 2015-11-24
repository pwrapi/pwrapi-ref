/* 
 * Copyright 2014-2015 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#ifndef _PWR_GROUP_H
#define _PWR_GROUP_H

#include <vector>
#include <string>

#include "status.h"
#include "pwrtypes.h"
#include "debug.h"
#include "object.h"
#include "util.h"

namespace PowerAPI {

class Cntxt;

class Grp {
  public:
    Grp( Cntxt* ctx, const std::string name ="" ) 
		: m_ctx(ctx), m_name(name) {}

    long size() { return m_list.size(); }
    bool empty() { return m_list.empty(); }

    Object* getObj( int i ) { return m_list[i]; }

    int add( Object* obj ) {
        m_list.push_back( obj );
        return PWR_RET_SUCCESS; 
    }

    const std::string& name() {
        return m_name;
    }
    
    int attrSetValue( PWR_AttrName type, void* ptr, Status* status )
	{
        for ( unsigned int i = 0; i < m_list.size(); i++ ) {
            int ret = m_list[i]->attrSetValue( type, ptr );
            if ( PWR_RET_SUCCESS != ret ) {
                status->add( m_list[i], type, ret );
            }
        }
        return !status->empty() ? PWR_RET_FAILURE : PWR_RET_SUCCESS;
    }

    int attrGetValue( PWR_AttrName type, void* ptr, PWR_Time ts[], Status* status )
	{
		assert(0);
    }

    int attrSetValues( int num, PWR_AttrName attr[], void* buf, Status* status )
    {
        for ( unsigned int i = 0; i < m_list.size(); i++ ) {

			m_list[i]->attrSetValues( num, attr, buf, status );
        }
        return !status->empty() ? PWR_RET_FAILURE : PWR_RET_SUCCESS;
    }

    int attrGetValues( int num, PWR_AttrName attr[], void* buf,
                                   		PWR_Time ts[], Status* status)
    {
        DBGX("num=%d size=%lu\n",num,m_list.size());
        uint64_t* ptr = (uint64_t*) buf;

        for ( unsigned int i = 0; i < m_list.size(); i++ ) {

            m_list[i]->attrGetValues( num, attr, ptr + i * num, ts, status ); 
        }

        return !status->empty() ? PWR_RET_FAILURE : PWR_RET_SUCCESS;
    }

    int remove( Object* obj ) {
        std::vector<Object*>::iterator iter = m_list.begin();
        for ( ; iter != m_list.end(); ++iter ) {
            if ( *iter == obj ) {
                m_list.erase( iter );
                break;
            }
        }
        return PWR_RET_SUCCESS;
    }

    Object* find( std::string name ) {
        DBGX("%s\n", name.c_str());
        
        for ( unsigned int i = 0; i < m_list.size(); i++ ) {
            Object* obj = m_list[i];
            if ( obj->parent() ) {
                std::string tmp = m_list[i]->parent()->name() + "." + name;
                DBGX("%s %s\n",tmp.c_str(), obj->name().c_str() );
                if ( 0 == tmp.compare( obj->name() ) ) { 
                    return obj;
                }
            }
        }
        return NULL;
    }

    Cntxt* getCntxt() { return m_ctx; }

  private:
    Cntxt*   					m_ctx;
    std::string 				m_name;
    std::vector<Object*> 	m_list;
};

}

#endif
