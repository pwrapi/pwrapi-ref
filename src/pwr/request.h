#ifndef _REQUEST_H
#define _REQUEST_H

#include <deque>
#include <pwrtypes.h>

namespace PowerAPI {

class Cntxt;
class CommReq;

class Request {

  public:
	Request( Cntxt* ctx, Callback callback = NULL, void* data = NULL ) : 
		retval( PWR_RET_SUCCESS ),
		m_cntxt( ctx),
		m_callback( callback ),
		m_data( data ) 
	{}

	virtual int wait( int* status ) = 0;
	virtual int check( int* status ) = 0;
	virtual bool finished() = 0;

	void* 		value;
	PWR_Time* 	timeStamp;
	unsigned int* count;
	int			retval;

  protected:
	Cntxt* 		m_cntxt;
	Callback 	m_callback;
	void*  		m_data;
};

}


#endif
