
#include <debug.h> 
#include "commCreateEvent.h" 
#include "router.h"

using namespace PWR_Router;

bool RtrCommCreateEvent::process( EventGenerator* _rtr, EventChannel* ec ) {
	Router& rtr = *static_cast<Router*>(_rtr);
	Router::Client* client = rtr.getClient( ec );
	DBGX("id=%lu\n",commID);

   	for ( unsigned int i = 0; i < members.size(); i++ ) {
       	DBGX("%s\n", members[i].c_str() );
		
		CommCreateEvent* ev = new CommCreateEvent();
		ev->commID = commID;

		ev->members.push_back( members[i].c_str() );
		rtr.sendEvent( members[i].c_str(), ev );
	}	

	client->addComm( commID, this );

	return false;
}
