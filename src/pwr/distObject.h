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

#ifndef _DIST_OBJECT_H
#define _DIST_OBJECT_H

#include "object.h"

namespace PowerAPI {

class Request;
class Status;
class DistComm;

class DistObject : public Object {

  public:
	DistObject( std::string name, PWR_ObjType type, Cntxt* ctx );

	bool isLocal() { return ! m_comm; }
	DistComm* getComm() { return m_comm; }
    virtual int attrGetValue( PWR_AttrName attr, void* buf, 
								PWR_Time* ts );
    virtual int attrSetValue( PWR_AttrName attr, void* buf );
	virtual int attrGetValues( int count, PWR_AttrName names[],
								void* buf, PWR_Time ts[], Status* );
    virtual int attrSetValues( int count, PWR_AttrName names[],
								void* buf, Status*  );

	virtual int attrGetValues(int count, PWR_AttrName [], void* buf,
								PWR_Time [], Request* );
	virtual int attrSetValues(int count, PWR_AttrName [], void* buf, Request* );

	virtual int attrGetValues( int count, PWR_AttrName names[],
							void* buf, PWR_Time ts[], Status*, Request* );
	virtual int attrSetValues( int count, PWR_AttrName names[],
							void* buf, Status*, Request* );

    virtual int attrStartLog( PWR_AttrName );
    virtual int attrStopLog( PWR_AttrName );
    virtual int attrGetSamples( PWR_AttrName name, PWR_Time* start,
					double period, unsigned int* count, void* buf );

    virtual int attrStartLog( PWR_AttrName, Request* );
    virtual int attrStopLog( PWR_AttrName, Request* );
    virtual int attrGetSamples( PWR_AttrName name, PWR_Time* start,
					double period, unsigned int* count, void* buf, Request* );

  private:
	DistComm* m_comm;
};

};

#endif
