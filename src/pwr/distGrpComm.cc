
#include "distGrpComm.h"
#include "debug.h"
#include "distCntxt.h"
#include "distObject.h"
#include "eventChannel.h"

using namespace PowerAPI;

DistGrpComm::DistGrpComm( DistCntxt* cntxt, 
					std::vector<DistObject*>& objs ) : 
		DistComm( cntxt )
{
    DBGX("num objects %lu\n", objs.size() );
    m_ec = cntxt->getEventChannel();
    assert(m_ec);

    CommCreateEvent* ev = new CommCreateEvent();
 	ev->commID = m_commID;

	for ( unsigned i = 0; i < objs.size(); i++ ) {
		DBGX("obj `%s` \n",objs[i]->name().c_str() );

		ev->members.push_back(  objs[i]->getComm()->getObjects() );
	} 

	m_ec->sendEvent( ev );
	delete ev;
}
