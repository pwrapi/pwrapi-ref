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

#ifndef _PWR_STAT_H
#define _PWR_STAT_H

#include <assert.h>

#include "pwr.h"
#include "group.h"
#include "object.h"
#include "attrInfo.h"

namespace PowerAPI {

class Cntxt;

class Stat {
  public:
	typedef double (*OpFuncPtr)(std::vector<double>&, int& pos );
	Stat( Cntxt* ctx, Object* obj, PWR_AttrName name, OpFuncPtr ptr,
															double hz ) 
	  : m_ctx( ctx), m_obj(obj), m_grp(NULL), m_attrName( name ), 
	    opPtr( ptr ), m_period( 1 / hz ),
		m_startTime(PWR_TIME_UNINIT), m_stopTime(PWR_TIME_UNINIT) 
    { 
        // for now limit stat to a leaf object with only 1 device
        if ( obj->children()->size() ) { 
            DBGX("have children\n");
            throw int ();
        }
        AttrInfo& info = obj->getAttrInfo( name );

        if ( ( 0 == info.devices.size() && ! info.comm ) || 
           ( 0 < info.devices.size() && info.comm ) ||
           ( 1 < info.devices.size() && ! info.comm ) ) {
            DBGX("devices\n");
            throw int ();
        }
    }

	Stat( Cntxt* ctx, Grp* grp, PWR_AttrName name, OpFuncPtr ptr, double hz ) 
	  : m_ctx( ctx), m_obj(NULL), m_grp(grp), m_attrName( name ),
	    opPtr( ptr ), m_period( 1/ hz), 
		m_startTime(PWR_TIME_UNINIT), m_stopTime(PWR_TIME_UNINIT)
    { 
        for ( unsigned i=0; i < grp->size(); i++ ) {
            if ( grp->getObj(i)->children()->size() ) {
                DBGX("have children\n");
                throw int ();
            }
            AttrInfo& info = grp->getObj(i)->getAttrInfo( name );
            if ( 1 != info.devices.size() ) {
                throw int ();
            }
        }
    }
	virtual ~Stat() {}

	virtual int start() = 0;
	virtual int stop() = 0; 
	virtual int clear() = 0;
	virtual int getValue( double* value, PWR_TimePeriod* statTimes ) = 0;
	virtual int getValues( double value[], PWR_TimePeriod statTimes[] ) = 0;

	Cntxt* getCtx() {
		return m_ctx;
	}


  protected:
	Cntxt*			m_ctx;
	Object*			m_obj;
	Grp*			m_grp;
	PWR_AttrName	m_attrName;
	OpFuncPtr		opPtr;
    double 			m_period;
    PWR_Time 		m_startTime;
    PWR_Time 		m_stopTime;
};

}

#endif
