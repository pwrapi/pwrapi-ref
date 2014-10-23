/* 
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 */

#ifndef _PWR_DEVICESTAT_H
#define _PWR_DEVICESTAT_H

#include "stat.h"

namespace PowerAPI {

class DeviceStat : public Stat {
  public:
	DeviceStat( Cntxt* ctx, ObjTreeNode* obj, PWR_AttrName name,
													PWR_AttrStat stat ) 
	  : Stat( ctx, obj, name, stat ) { }

	DeviceStat( Cntxt*ctx, Grp* grp, PWR_AttrName name, PWR_AttrStat stat ) 
	  : Stat( ctx, grp, name, stat ) { }

	virtual ~DeviceStat() {}

	virtual int start();
	virtual int stop();
	virtual int clear();
	virtual int getValue( double* value, PWR_StatTimes* statTimes );
	virtual int getValues( double value[], PWR_StatTimes statTimes[] );

  private:
};

}

#endif
