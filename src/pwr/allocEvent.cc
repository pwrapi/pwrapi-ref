#include <events.h>
#include <assert.h>
#include "debug.h"
#include "events.h"

static const char* _eventNames[] = {
	    FOREACH_ENUM(GENERATE_STRING)
};

Event* ctx_allocEvent( unsigned int type, SerialBuf& buf )
{
	DBG("`%s`\n",_eventNames[type]); 

	switch( (EventType) type ) {
	  default:
		assert(0);	
	  case CommResp:
		return new CommRespEvent( buf );
	  case CommLogResp:
		return new CommLogRespEvent( buf );
	  case CommGetSamplesResp:
		return new CommGetSamplesRespEvent( buf );
	}
	return NULL;
}
