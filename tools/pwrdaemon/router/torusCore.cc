#include "torusCore.h"
#include "router.h"
#include "routerSelect.h"
#include "allocEvent.h"

using namespace PWR_Router;

TorusCore::TorusCore( RouterCoreArgs* _args,  Router* router ) {
	TorusArgs& args = *static_cast<TorusArgs*>(_args);

	int nDims = args.dim.size();

    m_rtrLinks.resize( nDims );

    for ( size_t i = 0; i < nDims; i++ ) {
        EventChannel* ec;
        m_rtrLinks[i].resize(2);

        printf("%lu: posPort=%s ", i, args.dim[i].posPort.c_str() );
        printf("negSrvr=%s negSrvrPort=%s\n",
                args.dim[i].negSrvr.c_str(), args.dim[i].negSrvrPort.c_str() );

        std::string config = "server=" + args.dim[i].negSrvr +
                            " serverPort=" + args.dim[i].negSrvrPort;

        ec = getEventChannel( "TCP", allocRtrEvent, config, "router" );

        router->chanSelect().addChannel( ec, 
						new RouterData(ec, &router->m_router ) );

        m_rtrLinks[i][NEG_LINK] = ec;

        config = "listenPort=" + args.dim[i].posPort;
        ec = getEventChannel( "TCP", allocRtrEvent, config , "router" );
        router->chanSelect().addChannel( ec,
        				new AcceptData<EventData>( ec, &router->m_router ) );
        m_rtrLinks[i][POS_LINK] = ec;
    } 
}
