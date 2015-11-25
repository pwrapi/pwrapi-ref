#include "router.h"

using namespace PWR_Router;

Router::Client::Client( Router& rtr ) :m_rtr(rtr) {
	DBGX("\n");
}
Router::Client::~Client() {
	DBGX("\n");
    std::map<CommID,CommCreateEvent* >::iterator iter;
    for ( iter = m_commMap.begin(); iter != m_commMap.end(); ++iter ) {

   		CommCreateEvent* ev = iter->second;
        for ( unsigned int i = 0; i < ev->members.size(); i++ ) {
            DBGX("%s\n", ev->members[i].c_str() );
            CommDestroyEvent* d_ev = new CommDestroyEvent;
            d_ev->commID = ev->commID;
            m_rtr.sendEvent( ev->members[i].c_str(), d_ev );
            delete d_ev;
        }

        delete ev;
   }
}

void Router::Client::addComm( CommID id, CommCreateEvent* ev ) {
	assert( m_commMap.find( id ) == m_commMap.end() );
    m_commMap[id] = ev;
}

std::vector<ObjID>& Router::Client::getCommList( CommID id ) {
	return  m_commMap[id]->members;
}
