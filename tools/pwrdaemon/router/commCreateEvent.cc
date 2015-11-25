
#include <debug.h> 
#include "commCreateEvent.h" 
#include "router.h"

using namespace PWR_Router;

bool RtrCommCreateEvent::process( EventGenerator* _rtr, EventChannel* ec ) {
	Router& rtr = *static_cast<Router*>(_rtr);
	Router::Client* client = rtr.getClient( ec );
	DBGX("id=%llu\n",commID);

   	for ( unsigned int i = 0; i < members.size(); i++ ) {
       	DBGX("%s\n", members[i].c_str() );
		rtr.sendEvent( members[i].c_str(), this );
	}	

	client->addComm( commID, this );

	return false;
}
