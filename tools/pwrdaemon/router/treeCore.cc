/*
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#include <stdlib.h>
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
    for ( int i = 0; i < nLinks; i++ ) {
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

        DBGX("%d: myListenPort=%s \n", i, link.myListenPort.c_str() );
        DBGX("otherHost=%s otherHostListnPort=%s\n",
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

	if ( m_rtrLinks.empty() ) return NULL;

	for ( unsigned i = 1; i < m_rtrLinks.size(); i++ ) {
		if ( id >= m_rtrLinks[i].nidStart &&
			 id <= m_rtrLinks[i].nidStop ) {
			return m_rtrLinks[i].send;
		}
	}
	return m_rtrLinks[0].send; 
}
