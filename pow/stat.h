/* 
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 */

#ifndef _PWR_STAT_H
#define _PWR_STAT_H

#include <assert.h>

#include "pow.h"
#include "group.h"
#include "objTreeNode.h"

namespace PowerAPI {

class Grp;
class ObjTreeNode;
class Cntxt;

class Stat {
  public:
	Stat( Cntxt* ctx, ObjTreeNode* obj, PWR_AttrName name, PWR_AttrStat stat ) 
	  : m_ctx( ctx), m_obj(obj), m_grp(NULL), 
			m_attrName( name ), m_attrStat( stat ) { }

	Stat( Cntxt* ctx, Grp* grp, PWR_AttrName name, PWR_AttrStat stat ) 
	  : m_ctx( ctx), m_obj(NULL), m_grp(grp), 
			m_attrName( name ), m_attrStat( stat ) { }
	virtual ~Stat() {}

	virtual int start() = 0;
	virtual int stop() = 0; 
	virtual int clear() = 0;
	virtual int getValue( double* value, PWR_StatTimes* statTimes ) = 0;
	virtual int getValues( double value[], PWR_StatTimes statTimes[] ) = 0;

	Cntxt* getCtx() {
		return m_ctx;
	}

  protected:
	Cntxt*			m_ctx;
	ObjTreeNode*	m_obj;
	Grp*			m_grp;
	PWR_AttrName	m_attrName;
	PWR_AttrStat    m_attrStat;
};

}

#endif
