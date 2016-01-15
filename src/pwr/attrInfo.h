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

#ifndef _ATTR_INFO_H
#define _ATTR_INFO_H

#include <deque>
#include <communicator.h>

namespace PowerAPI {

class Device; 
class Communicator;

class AttrInfo {
  public:
	typedef void (*OpFuncPtr)( void* out, void* in, size_t num );
	typedef PWR_Time (*TimeFuncPtr)( std::vector<PWR_Time> );

	AttrInfo( OpFuncPtr fptr, TimeFuncPtr tptr, ValueOp op) : 
		comm( NULL ), operation(fptr), calcTime(tptr), valueOp(op) {}
	virtual ~AttrInfo() {}

	virtual bool isValid() { 
		return ! devices.empty() || comm; 
	}   

	std::vector<Device*> devices;
	Communicator*		comm;
	OpFuncPtr			operation;
	TimeFuncPtr 		calcTime;
	ValueOp				valueOp;	
};

}

#endif
