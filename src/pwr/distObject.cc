#include "distObject.h"
#include "attrInfo.h"
#include "distRequest.h"
#include "distComm.h"
#include "status.h"
#include "debug.h"

using namespace PowerAPI;

int DistObject::attrGetValue( PWR_AttrName attr, void* buf,
                                PWR_Time* ts )
{
    int retval;
	Status	status;
    DBGX("\n");

	retval = attrGetValues( 1, &attr, buf, ts, &status );
	if ( retval != PWR_RET_SUCCESS ) {
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
	if ( retval != PWR_RET_SUCCESS ) {
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
	DistRequest req( getCntxt() );

	// we don't care about the return value because errors will be
	// flagged in the status structure
	attrGetValues( count, names, buf, ts, status, &req );

	retval = req.wait( status );
	assert( retval == PWR_RET_SUCCESS );

    return status->empty() ? PWR_RET_SUCCESS : PWR_RET_FAILURE; 
}

int DistObject::attrSetValues( int count, PWR_AttrName names[],
                                void* buf, Status* status )
{
    int retval;
    DBGX("\n");
	DistRequest req( getCntxt() );

	// we don't care about the return value because errors will be
	// flagged in the status structure
	attrSetValues( count, names, buf, status, &req );

	retval = req.wait( status );
	assert( retval == PWR_RET_SUCCESS );

    return status->empty() ? PWR_RET_SUCCESS : PWR_RET_FAILURE; 
}

//=============================================================================

int DistObject::attrGetValue( PWR_AttrName attr, void* buf,
                                PWR_Time* ts, Request* req )
{
	int retval;
	Status status;
    DBGX("\n");
	retval = attrGetValues( 1, &attr, buf, ts, &status, req );
	if ( retval != PWR_RET_SUCCESS ) {
        PWR_AttrAccessError error;
        retval = status.pop( &error );
        assert( retval == PWR_RET_SUCCESS );
        return error.error;
    }
    return retval;
}

int DistObject::attrSetValue( PWR_AttrName attr, void* buf, Request* req )
{
    int retval;
	Status status;
    DBGX("\n");
	retval = attrSetValues( 1, &attr, buf, &status, req );
	if ( retval != PWR_RET_SUCCESS ) {
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
	req->value = buf;
	req->timeStamp = ts;

	// we don't care about the return value because errors will be
	// flagged in the status structure
    Object::attrGetValues( count, names, buf, ts, status );

	for ( int i = 0; i < count; i++ ) {
		AttrInfo* info = m_attrInfo[ names[i] ];
		if ( info->comm ) {
			DistCommReq* commReq = 
						new DistGetCommReq(static_cast<DistRequest*>(req));	
			distReq->insert( commReq );
			info->comm->getValue( names[i], info->valueOp, commReq );
		}
	}

	retval = status->empty() ? PWR_RET_SUCCESS : PWR_RET_FAILURE; 

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

	for ( int i = 0; i < count; i++ ) {
		AttrInfo* info = m_attrInfo[ names[i] ];
		if ( info->comm ) {
			DistCommReq* commReq = 
						new DistSetCommReq(static_cast<DistRequest*>(req));	
			distReq->insert( commReq );
			info->comm->setValue( names[i], buf, commReq );
		}
	}
	retval = status->empty() ? PWR_RET_SUCCESS : PWR_RET_FAILURE; 
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
	DistRequest req( getCntxt() );

	DBGX("\n");

	retval = attrStartLog( attr, &req ); 
	if ( retval != PWR_RET_SUCCESS ) {
		return retval;
	}	
	int status;
	req.wait( &status );
	assert ( retval == PWR_RET_SUCCESS );

	return status;
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
	DistRequest req( getCntxt() );

	DBGX("\n");

	retval = attrStopLog( attr, &req ); 
	if ( retval != PWR_RET_SUCCESS ) {
		return retval;
	}	
	int status;
	req.wait( &status );
	assert ( retval == PWR_RET_SUCCESS );

	return status;
}

int DistObject::attrGetSamples( PWR_AttrName attr, PWR_Time* ts,
        		double period, unsigned int* count, void* buf, Request* req )
{
	int retval;
	req->value = buf;
	req->timeStamp = ts;
	req->count = count;

	DBGX("period=%f count=%d\n", period, *count );
	DistRequest* distReq = static_cast<DistRequest*>(req);
	retval = Object::attrGetSamples( attr, ts, period, count, buf ); 
	if ( retval != PWR_RET_SUCCESS ) {
		return retval;
	}	
	DBGX("ts=%llu\n",*ts);

	AttrInfo* info = m_attrInfo[ attr ];
	if ( info->comm ) {
		DistCommReq* commReq = 
					new DistGetSamplesCommReq(static_cast<DistRequest*>(req));	
		distReq->insert( commReq );
		info->comm->getSamples( attr, ts, period, *count, commReq );
	}
	if ( retval == PWR_RET_SUCCESS && distReq->finished() ) {
		if ( distReq->execCallback( ) ) {
			delete req;
		}
	}
	return PWR_RET_SUCCESS;
}

int DistObject::attrGetSamples( PWR_AttrName attr, PWR_Time* ts,
                      double period, unsigned int* count, void* buf )
{
	int retval;
	DistRequest req( getCntxt() );

	DBGX("\n");

	retval = attrGetSamples( attr, ts, period, count, buf, &req ); 
	if ( retval != PWR_RET_SUCCESS ) {
		return retval;
	}	
	int status;
	req.wait( &status );
	assert ( retval == PWR_RET_SUCCESS );

	return status;
}

