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

#ifndef _OBJECT_H
#define _OBJECT_H

#include <assert.h>

#include <map>
#include <string>
#include <vector>

#include "pwrtypes.h"
#include "cntxt.h"

namespace PowerAPI {

class Grp;
class Status;
class AttrInfo;
class Request;

class Object {

  public:
	Object( std::string name, PWR_ObjType type, Cntxt* ctx );
	virtual ~Object();

	PWR_ObjType type() { return m_objType; } 
	std::string& name() { return m_name; } 
	Cntxt* getCntxt() { return m_cntxt; } 

	virtual Object* parent();
	virtual Grp* children();

	virtual AttrInfo& getAttrInfo( PWR_AttrName attr ) {
		return *m_attrInfo[attr];
	}
	virtual bool attrIsValid( PWR_AttrName );

	virtual int attrGetValue( PWR_AttrName attr, void* buf, PWR_Time* ts );
	virtual int attrSetValue( PWR_AttrName attr, void* buf );
	virtual int attrGetValues( int count, PWR_AttrName names[], void* buf,
							PWR_Time ts[], Status* );
	virtual int attrSetValues( int count, PWR_AttrName names[], void* buf,
							Status* );

	virtual int attrGetValues( int count, PWR_AttrName names[], void* buf,
							PWR_Time ts[], Status*, Request*  ) {
		assert(0);
	}
	virtual int attrSetValues( int count, PWR_AttrName names[], void* buf,
							Status*, Request* ) { 
		assert(0);
	}
	virtual int attrGetValue( PWR_AttrName attr, void* buf,
							PWR_Time* ts, Request* req ) { 
		assert(0);
	}
	virtual int attrSetValue( PWR_AttrName attr, void* buf, 
							Request* req ) { 
		assert(0);
	}

    virtual int attrStartLog( PWR_AttrName );
    virtual int attrStopLog( PWR_AttrName );
	virtual int attrGetSamples( PWR_AttrName name, PWR_Time* start,
					double period, unsigned int* count, void* buf );

    virtual int attrStartLog( PWR_AttrName, Request* ) {	
		assert(0);
	}
    virtual int attrStopLog( PWR_AttrName, Request* ) {
		assert(0);
	}
	virtual int attrGetSamples( PWR_AttrName name, PWR_Time* start, 
					double period, unsigned int* count, void* buf, Request* ) {
		assert(0);
	}

  protected:

	int attrGetValuesDevice( AttrInfo&, PWR_AttrName, void* buf, PWR_Time* );
	int attrSetValuesDevice( AttrInfo&, PWR_AttrName, void* buf );	

	std::string     m_name;
	PWR_ObjType	    m_objType;
	Cntxt* 			m_cntxt;
	Object*			m_parent;
	Grp*			m_children;
	std::vector< AttrInfo* > m_attrInfo; 
};

};

#endif
