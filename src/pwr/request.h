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

class Request {

  public:
	Request( Cntxt* ctx, Callback callback = NULL, void* data = NULL ) : 
		value(1),
		timeStamp(1),
		retval( PWR_RET_SUCCESS ),
		m_cntxt( ctx),
		m_callback( callback ),
		m_data( data ) 
	{}
	virtual ~Request() {}

	virtual int wait( int* status ) = 0;
	virtual int check( int* status ) = 0;
	virtual bool finished() = 0;

	std::vector<void*> 		value;
	std::vector<PWR_Time*> 	timeStamp;
	unsigned int* count;
	int retval;

  protected:
	Cntxt* 		m_cntxt;
	Callback 	m_callback;
	void*  		m_data;
};

}


#endif
