#include "treeCore.h"
#include "router.h"
#include "routerSelect.h"
#include "allocEvent.h"

using namespace PWR_Router;

TreeCore::TreeCore( RouterCoreArgs* _args,  Router* router ) 
{
	TreeArgs& args = *static_cast<TreeArgs*>(_args);
	
	int nLinks = args.links.size();

    m_rtrLinks.resize( nLinks );
    for ( size_t i = 0; i < nLinks; i++ ) {
		Link& link = args.links[i];
		if ( link.myListenPort.empty() ) {
			continue;
		}
		DBGX("%s %s %s\n",link.myListenPort.c_str(),
					link.otherHost.c_str(),
					link.otherHostListenPort.c_str() );

		if ( ! link.nidStart.empty() ) {
			DBGX("nidStart=%s nidStop=%s\n",link.nidStart.c_str(),link.nidStop.c_str());
		}
        EventChannel* ec;

        printf("%lu: myListenPort=%s ", i, link.myListenPort.c_str() );
        printf("otherHost=%s otherHostListnPort=%s\n",
                link.otherHost.c_str(), link.otherHostListenPort.c_str() );

        std::string config = "server=" + link.otherHost +
                            " serverPort=" + link.otherHostListenPort;

        ec = getEventChannel( "TCP", allocRtrEvent, config, "router" );

        router->chanSelect().addChannel( ec, 
						new RouterData(ec, &router->m_router ) );

        m_rtrLinks[i].send = ec;

        config = "listenPort=" + link.myListenPort;
        ec = getEventChannel( "TCP", allocRtrEvent, config , "router" );
        router->chanSelect().addChannel( ec,
        				new AcceptData<EventData>( ec, &router->m_router ) );
        m_rtrLinks[i].recv = ec;

		if ( i > 0 ) {
			m_rtrLinks[i].nidStart = atoi( link.nidStart.c_str() );
			m_rtrLinks[i].nidStop = atoi( link.nidStop.c_str() );
		}
    } 
}

EventChannel* TreeCore::getChannel( RouterID id )
{
	DBGX("router ID %d\n",id);
	for ( unsigned i = 1; i < m_rtrLinks.size(); i++ ) {
		if ( id >= m_rtrLinks[i].nidStart &&
			 id <= m_rtrLinks[i].nidStop ) {
			return m_rtrLinks[i].send;
		}
	}
	return m_rtrLinks[0].send; 
}
