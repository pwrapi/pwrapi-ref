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

#ifndef _PWR_DEVICESTAT_H
#define _PWR_DEVICESTAT_H

#include "stat.h"

namespace PowerAPI {

class DeviceStat : public Stat {
  public:
	DeviceStat( Cntxt* ctx, Object* obj, PWR_AttrName name,
											OpFuncPtr ptr, double hz ) 
	  : Stat( ctx, obj, name, ptr, hz ), m_isLogging(false) { }

	DeviceStat( Cntxt*ctx, Grp* grp, PWR_AttrName name, OpFuncPtr ptr, double hz ) 
	  : Stat( ctx, grp, name, ptr, hz ), m_isLogging(false) { }

	virtual ~DeviceStat();

	virtual int start();
	virtual int stop();
	virtual int clear();
	virtual int getValue( double* value, PWR_TimePeriod* statTimes );
	virtual int getValues( double value[], PWR_TimePeriod statTimes[] );

  private:
	int startObj();
	int startGrp();
	int stopObj();
	int stopGrp();
	int objGetValue( Object*, double* value, PWR_TimePeriod* statTimes );
	bool m_isLogging;
};

}

#endif
