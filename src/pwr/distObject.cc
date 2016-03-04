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

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "distObject.h"
#include "attrInfo.h"
#include "distRequest.h"
#include "distComm.h"
#include "status.h"
#include "debug.h"
#include "util.h"

using namespace PowerAPI;

DistObject::DistObject( std::string name, PWR_ObjType type, Cntxt* ctx ) :
        Object( name, type, ctx ), m_comm( NULL )
{
	for ( int attr = PWR_ATTR_PSTATE; attr < PWR_NUM_ATTR_NAMES; attr++ ) {
		if ( m_attrInfo[ attr ]->comm ) {
    		DBGX("%s comm=%p\n",attrNameToString((PWR_AttrName)attr),
									m_attrInfo[ attr ]->comm); 
    		m_comm = static_cast<DistComm*>(m_attrInfo[0]->comm);
			break;
		} 
	}	

    // for now all attributes for an object must serviced the same way
    for ( int attr = PWR_ATTR_PSTATE; attr < PWR_NUM_ATTR_NAMES; attr++ ) {
        assert( ! ( m_attrInfo[attr]->comm &&
                        ! m_attrInfo[attr]->devices.empty() ) );
        if ( ! m_comm ) {
            m_comm = static_cast<DistComm*>(m_attrInfo[attr]->comm);

        } else if ( m_attrInfo[attr]->comm ) {
            assert( m_comm == m_attrInfo[attr]->comm );
        }
    }
    DBGX("m_comm %p\n",m_comm);
}

int DistObject::attrGetValue( PWR_AttrName attr, void* buf,
                                PWR_Time* ts )
{
    int retval;
	Status	status;
    DBGX("\n");

	retval = attrGetValues( 1, &attr, buf, ts, &status );
	if ( retval == PWR_RET_STATUS ) {
        PWR_AttrAccessError error;
        retval = status.pop( &error );
        assert( retval == PWR_RET_SUCCESS );
        retval = error.error;
	}

    return retval;
}

int DistObject::attrSetValue( PWR_AttrName attr, void* buf )
{
    int retval;
	Status	status;
    DBGX("\n");

	retval = attrSetValues( 1, &attr, buf, &status );
	if ( retval == PWR_RET_STATUS ) {
        PWR_AttrAccessError error;
        retval = status.pop( &error );
        assert( retval == PWR_RET_SUCCESS );
        retval = error.error;
	}

    return retval;
}

int DistObject::attrGetValues( int count, PWR_AttrName names[],
                                void* buf, PWR_Time ts[], Status* status )
{
    int retval;
    DBGX("\n");
	DistRequest req( getCntxt(), status );

	// we don't care about the return value because errors will be
	// flagged in the status structure
	attrGetValues( count, names, buf, ts, status, &req );

	retval = req.wait( );
	if( retval != PWR_RET_SUCCESS ) {
		return retval;
	}

    return status->empty() ? PWR_RET_SUCCESS : PWR_RET_STATUS; 
}

int DistObject::attrSetValues( int count, PWR_AttrName names[],
                                void* buf, Status* status )
{
    int retval;
    DBGX("\n");
	DistRequest req( getCntxt(), status );

	// we don't care about the return value because errors will be
	// flagged in the status structure
	attrSetValues( count, names, buf, status, &req );

	retval = req.wait( );
	if( retval != PWR_RET_SUCCESS ) {
		return retval;
	}

    return status->empty() ? PWR_RET_SUCCESS : PWR_RET_STATUS; 
}

//=============================================================================

int DistObject::attrGetValues( int count, PWR_AttrName attr[], void* buf,
                                PWR_Time ts[], Request* req )
{
	int retval;
	Status status;
    DBGX("\n");
	retval = attrGetValues( count, attr, buf, ts, &status, req );
	if ( retval == PWR_RET_STATUS ) {
        PWR_AttrAccessError error;
        retval = status.pop( &error );
        assert( retval == PWR_RET_SUCCESS );
        return error.error;
    }
    return retval;
}

int DistObject::attrSetValues( int count, PWR_AttrName attr[], void* buf,
								Request* req )
{
    int retval;
	Status status;
    DBGX("\n");
	retval = attrSetValues( count, attr, buf, &status, req );
	if ( retval == PWR_RET_STATUS ) {
        PWR_AttrAccessError error;
        retval = status.pop( &error );
        assert( retval == PWR_RET_SUCCESS );
        return error.error;
    }
    return retval;
}

int DistObject::attrGetValues( int count, PWR_AttrName names[],
                        void* buf, PWR_Time ts[], Status* status, Request* req )
{
    int retval;
	DistRequest* distReq = static_cast<DistRequest*>(req);
    DBGX("\n");
	req->value[0] = buf;
	req->timeStamp[0] = ts;

	// we don't care about the return value because errors will be
	// flagged in the status structure
    Object::attrGetValues( count, names, buf, ts, status );

	AttrInfo* info = m_attrInfo[ names[0] ];
	std::vector<ValueOp> valueOp(count);
	valueOp[0] = info->valueOp; 

	for ( int i = 1; i < count; i++ ) {
		assert( info->comm == m_attrInfo[ names[i] ]->comm );
		valueOp[i] = m_attrInfo[ names[i] ]->valueOp; 
	}

	if ( info->comm ) {

		DistCommReq* commReq = 
					new DistGetCommReq(static_cast<DistRequest*>(req));	
		distReq->insert( commReq );
		info->comm->getValues( count, names, &valueOp[0], commReq );
	}

	retval = status->empty() ? PWR_RET_SUCCESS : PWR_RET_STATUS; 

	// Need to sort or the relationsip between a Status and Request. If the function has
	// a Request argument shoud the Status object be included in the Request object and
	// not be passed as an argument
	if ( retval == PWR_RET_SUCCESS && distReq->finished() ) {
		if ( distReq->execCallback( ) ) {
			delete distReq;
		}
	}

    return retval;
}


int DistObject::attrSetValues( int count, PWR_AttrName names[],
                                void* buf, Status* status, Request* req )
{
    int retval;
	DistRequest* distReq = static_cast<DistRequest*>(req);
    DBGX("\n");

	// we don't care about the return value because errors will be
	// flagged in the status structure
    Object::attrSetValues( count, names, buf, status );

	AttrInfo* info = m_attrInfo[ names[0] ];

	for ( int i = 1; i < count; i++ ) {
		assert( info->comm == m_attrInfo[ names[i] ]->comm );
	}

	if ( info->comm ) {
		DistCommReq* commReq = 
					new DistSetCommReq(static_cast<DistRequest*>(req));	
		distReq->insert( commReq );
		info->comm->setValues( count, names, buf, commReq );
	}

	retval = status->empty() ? PWR_RET_SUCCESS : PWR_RET_STATUS; 
	if ( retval == PWR_RET_SUCCESS && distReq->finished() ) {
		if ( distReq->execCallback( ) ) {
			delete distReq;
		}
	}

    return retval;
}

//=============================================================================

int DistObject::attrStartLog( PWR_AttrName attr, Request* req )
{
	int retval;
	DBGX("\n");
	DistRequest* distReq = static_cast<DistRequest*>(req);
	retval = Object::attrStartLog( attr ); 
	if ( retval != PWR_RET_SUCCESS ) {
		return retval;
	}	

	AttrInfo* info = m_attrInfo[ attr ];
	if ( info->comm ) {
		DistCommReq* commReq = 
					new DistStartLogCommReq(static_cast<DistRequest*>(req));	
		distReq->insert( commReq );
		info->comm->startLog( attr, commReq );
	}
	if ( retval == PWR_RET_SUCCESS && distReq->finished() ) {
		if ( distReq->execCallback( ) ) {
			delete distReq;
		}
	}
	return PWR_RET_SUCCESS;
}

int DistObject::attrStartLog( PWR_AttrName attr )
{
	int retval;
	Status status;
	DistRequest req( getCntxt(), & status );

	DBGX("\n");

	retval = attrStartLog( attr, &req ); 
	if ( retval != PWR_RET_SUCCESS ) {
		return retval;
	}	

	req.wait( );

	if ( ! status.empty() ) {
		PWR_AttrAccessError error;
		status.pop( &error );
		retval = error.error;
	}

	return retval;
}

int DistObject::attrStopLog( PWR_AttrName attr, Request* req )
{
	int retval;
	DBGX("\n");
	DistRequest* distReq = static_cast<DistRequest*>(req);
	retval = Object::attrStopLog( attr ); 
	if ( retval != PWR_RET_SUCCESS ) {
		return retval;
	}	

	AttrInfo* info = m_attrInfo[ attr ];
	if ( info->comm ) {
		DistCommReq* commReq = 
					new DistStopLogCommReq(static_cast<DistRequest*>(req));	
		distReq->insert( commReq );
		info->comm->stopLog( attr, commReq );
	}
	if ( retval == PWR_RET_SUCCESS && distReq->finished() ) {
		if ( distReq->execCallback( ) ) {
			delete distReq;
		}
	}
	return PWR_RET_SUCCESS;
}

int DistObject::attrStopLog( PWR_AttrName attr )
{
	int retval;
	Status status;
	DistRequest req( getCntxt(), &status );

	DBGX("\n");

	retval = attrStopLog( attr, &req ); 
	if ( retval != PWR_RET_SUCCESS ) {
		return retval;
	}	

	req.wait(  );

    if ( ! status.empty() ) {
        PWR_AttrAccessError error;
        status.pop( &error );
        retval = error.error;
    }

	return retval; 
}

int DistObject::attrGetSamples( PWR_AttrName attr, PWR_Time* start,
        		double period, unsigned int* count, void* buf, Request* req )
{
	int retval;

	DBGX("period=%f count=%d\n", period, *count );
	DistRequest* distReq = static_cast<DistRequest*>(req);

	retval = Object::attrGetSamples( attr, start, period, count, buf ); 
	if ( retval != PWR_RET_SUCCESS ) {
		return retval;
	}	

	AttrInfo* info = m_attrInfo[ attr ];
	if ( info->comm ) {
	
		req->value[0] = buf;
		req->timeStamp[0] = start;
		req->count = count;

		DistCommReq* commReq = 
					new DistGetSamplesCommReq(static_cast<DistRequest*>(req));	
		distReq->insert( commReq );
		info->comm->getSamples( attr, *start, period, *count, commReq );
	}
	if ( retval == PWR_RET_SUCCESS && distReq->finished() ) {
		if ( distReq->execCallback( ) ) {
			delete req;
		}
	}
	return PWR_RET_SUCCESS;
}

int DistObject::attrGetSamples( PWR_AttrName attr, PWR_Time* start,
                      double period, unsigned int* count, void* buf )
{
	int retval;
	Status status;
	DistRequest req( getCntxt(), &status );

	DBGX("\n");

	retval = attrGetSamples( attr, start, period, count, buf, &req ); 
	if ( retval != PWR_RET_SUCCESS ) {
		return retval;
	}	
	req.wait();
    if ( ! status.empty() ) {
        PWR_AttrAccessError error;
        status.pop( &error );
        retval = error.error;
    }

	return retval;
}

