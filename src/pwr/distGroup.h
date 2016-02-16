
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

#ifndef _DIST_GROUP_H
#define _DIST_GROUP_H

#include <assert.h>

#include "group.h"

namespace PowerAPI {

class Cntxt;
class DistObject;

class DistGrp : public Grp {
  public:
    DistGrp( Cntxt* ctx, const std::string name ="" ) : Grp( ctx, name ) {
	}
	virtual int add( Object* obj );

	virtual int attrSetValue( PWR_AttrName type, void* ptr, Status* status );
	virtual int attrGetValue( PWR_AttrName type, void* ptr, PWR_Time ts[],
						 	Status* status );
	virtual int attrSetValues( int num, PWR_AttrName attr[], void* buf,
							Status* status );
	virtual int attrGetValues( int num, PWR_AttrName attr[], void* buf,
                                        PWR_Time ts[], Status* status);
	
  private:
	std::vector<DistObject*>  m_distObjs;
};

}

#endif