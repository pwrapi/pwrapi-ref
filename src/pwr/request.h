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

#ifndef _REQUEST_H
#define _REQUEST_H

#include <vector>
#include <deque>
#include <pwrtypes.h>

namespace PowerAPI {

class Cntxt;
class Status;

class Request {

  public:
	Request( Cntxt* ctx, Status* status, 
				Callback callback = NULL, void* data = NULL ) : 
		value(1),
		timeStamp(1),
		m_cntxt( ctx),
		m_status( status ),
		m_callback( callback ),
		m_data( data ) 
	{}
	virtual ~Request() {}

	virtual int wait( ) = 0;
	virtual bool finished() = 0;

	// getAttr
	std::vector<void*> 		value;
	std::vector<PWR_Time*> 	timeStamp;

	// where to put the number of samples returned 
	unsigned int* count;
	
  protected:
	Cntxt* 		m_cntxt;
	Status* 	m_status;
	Callback 	m_callback;
	void*  		m_data;
};

}


#endif
