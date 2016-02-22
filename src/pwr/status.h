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

#ifndef _PWR_STATUS_H
#define _PWR_STATUS_H

#include <deque>

#include "pwrtypes.h"

namespace PowerAPI {

class Object; 

class Status {

  public:
    bool empty() {
        return m_info.empty();
    }
    int pop( PWR_AttrAccessError* ptr ) {
        if ( m_info.empty() ) return PWR_RET_EMPTY;
        *ptr = m_info.front();
        m_info.pop_front();
        return PWR_RET_SUCCESS;
    }

    void add( Object* obj, PWR_AttrName name, int error  ) {
        PWR_AttrAccessError tmp;
        tmp.obj = obj;
        tmp.name = name;
        tmp.error = error;
        m_info.push_back( tmp ); 
    }
    int clear() {
        m_info.clear();
        return PWR_RET_SUCCESS;
    }

  private:
    
    std::deque<PWR_AttrAccessError> m_info;
};

}

#endif
