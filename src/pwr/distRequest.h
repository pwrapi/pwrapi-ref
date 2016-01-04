#ifndef _DIST_REQUEST_H
#define _DIST_REQUEST_H

#include <set>
#include <request.h>


class CommRespEvent;
class CommGetSamplesRespEvent;

namespace PowerAPI {

class Status;
class DistCommReq;

class DistRequest : public Request {

  public:
	DistRequest( Cntxt* ctx,
			Callback callback = NULL, void* data = NULL ) :
		Request( ctx, callback, data )
	{}
	~DistRequest( );

	int wait( Status* status );
	int check( int* status );
	int wait( int* status );

	bool finished() { 
		return m_commReqs.empty();
	}
	virtual bool execCallback( ) {
        if ( m_callback) {
            (m_callback)(m_data,retval);
        }
		return false;
    }

	void getSamples( DistCommReq*, CommGetSamplesRespEvent* );
	void setStatus( DistCommReq*, CommRespEvent* );
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
