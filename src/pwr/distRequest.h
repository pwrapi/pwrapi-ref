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

#ifndef _DIST_REQUEST_H
#define _DIST_REQUEST_H

#include <set>
#include <request.h>


struct CommRespEvent;
struct CommLogRespEvent;
struct CommGetSamplesRespEvent;

namespace PowerAPI {

class Status;
class DistCommReq;

class DistRequest : public Request {

  public:
	DistRequest( Cntxt* ctx, Status* status,
			Callback callback = NULL, void* data = NULL ) :
		Request( ctx, status, callback, data )
	{}
	~DistRequest( );

	int wait( );

	bool finished() { 
		return m_commReqs.empty();
	}
	virtual bool execCallback( ) {
        if ( m_callback) {
            (m_callback)(m_data);
        }
		return false;
    }

	void setRetval( DistCommReq*, CommLogRespEvent* );
	void getSamples( DistCommReq*, CommGetSamplesRespEvent* );

	void getValue( DistCommReq*, CommRespEvent* );
	void setValue( DistCommReq*, CommRespEvent* );
	void insert( DistCommReq* req ) {
		m_commReqs.insert( req );
	}

  protected:

	std::set<DistCommReq*> m_commReqs;
};

}


#endif
