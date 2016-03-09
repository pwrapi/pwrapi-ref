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

#ifndef _ROUTER_H
#define _ROUTER_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <map>
#include <vector>
#include <set>
#include "routerSelect.h"
#include "debug.h"
#include "events.h"
#include <config.h>
#include <tcpEventChannel.h>
#include "routerEvent.h"
#include "commCreateEvent.h"
#include "routerCore.h"
#include "impTypes.h"

typedef uint32_t ServerID;

#define RTR_ID( x ) (RouterID)(x >> 32) 
#define SERVER_ID( x ) (ServerID) (x & 0xffffffff)
#define APP_ID( rtr, server ) (AppID) (unsigned)rtr << 32 | (unsigned)server

class EventChannel;
class Config;

namespace PWR_Router {


struct Args {
    Args( ) : rtrId(-1), coreArgs(NULL) { }
    RouterID   	rtrId;
	std::string routeTable;
    std::string	serverPort;
    std::string clientPort;

	RouterCoreArgs* coreArgs;
};

class Router : public EventGenerator {

	typedef void (Router::*ChanFuncPtr)(EventChannel*);

	class Chan : public ChanBase {
	  public:
		Chan( Router* obj, ChanFuncPtr add, ChanFuncPtr del )  :
			m_rtr(*obj), m_add( add ), m_del( del )
	   	{}

		void add( EventChannel* chan ) {
			(m_rtr.*m_add)( chan );
		}			
		void del( EventChannel* chan ) {
			(m_rtr.*m_del)( chan );
		}				
	  private:
		Router& 	m_rtr;
		ChanFuncPtr m_add;
		ChanFuncPtr m_del;
	};

  public:
	class Client {
	  public:
		Client( Router& rtr );
		~Client();
		void addComm( CommID id, CommCreateEvent* ev );
		std::vector< std::vector< ObjID > >& getCommList( CommID id );

	  private:	  
		std::map<CommID,CommCreateEvent* > m_commMap;
		Router& 		m_rtr;
	};

	class Server {
	  public:
		Server( Router& rtr ) :m_rtr(rtr) {
			DBGX("\n");
		}
		~Server() {
			m_rtr.delServer(m_name);
		}
		void initName( std::string name ) {
			m_name = name;
		}
	  private:
		Router& 		m_rtr;
		std::string 	m_name;
	};

  public:

	Router( int, char* [] );

	ServerID addServer( std::string name, EventChannel* ec ) {
		AppID id = findRoute( name );

		DBGX("rootObj=`%s` rtrId=%d serverId=%d\n",
			name.c_str(), RTR_ID(id), SERVER_ID(id)  );

		assert( (unsigned) -1 != id );

		m_localMap[ SERVER_ID(id) ] = ec;
		m_serverMap[ ec ]->initName(name);
		return SERVER_ID(id);
	}

	void delServer( std::string name ) {
		DBGX("%s\n",name.c_str());
		//assert( m_localMap.find(name) != m_localMap.end() );
		m_localMap.erase( 0 );
	}

	Client* getClient( EventChannel* ec ) {
		assert ( m_clientMap.find(ec) != m_clientMap.end() );
		return m_clientMap[ec];
	}

	void sendEvent( AppID, Event* );
	void sendEvent( ObjID, Event* );

	AppID findDestApp( ObjID );
	int work();

	Args m_args;
	Chan m_client;
	Chan m_server;
	Chan m_router;

	ChannelSelect& chanSelect() { return *m_chanSelect; }

	void doPending( ServerID );

  private:

	EventChannel* findRtrChan( RouterID );
	EventChannel* findServerChan( ServerID );

	AppID findRoute( ObjID id ) {
		AppID retval = -1;
		if ( m_routeTable.find(id) != m_routeTable.end() ) {
			retval = m_routeTable[id];
		}
		DBGX("name=`%s` AppID=%"PRIx64"\n", id.c_str(), retval  )
    	return retval;
	}

	void addClientChan( EventChannel* ec) {
		DBGX("ec=%p\n",ec);
		m_clientMap[ec] = new Client(*this);
	}			

	void delClientChan( EventChannel* ec ) {
		DBGX("ec=%p\n",ec);
		delete m_clientMap[ec];
		m_clientMap.erase(ec);
	}			

	void addServerChan( EventChannel* ec) {
		DBGX("ec=%p\n",ec);
		m_serverMap[ec] = new Server(*this);
	}			

	void delServerChan( EventChannel* ec ) {
		DBGX("ec=%p\n",ec);
		delete m_serverMap[ec];
		m_serverMap.erase(ec);
	}			

	void addRouterChan( EventChannel* ec) {
		DBGX("\n");
	}			
	void delRouterChan( EventChannel* ec) {
		DBGX("\n");
	}			

	void initRouteTable( std::string file );


  private:
	ChannelSelect* 			 		m_chanSelect;

	std::map<EventChannel*,Server*>	m_serverMap;
	std::map<EventChannel*,Client*>	m_clientMap;

	std::map<ServerID,EventChannel*> m_localMap;
	RouterCore* 					m_routerCore;
	std::map< std::string, AppID >  m_routeTable;
	std::map< AppID, std::deque< Event*> > 	m_pendingEvents;
};

struct CommReqInfo {
   	EventChannel*   src;
    CommEvent*      ev;
	std::vector<size_t>	grpInfo;

	size_t			pending;

	std::vector<ValueOp>		valueOp;
	std::vector< std::vector< CommRespEvent* > > 	respQ; 
	CommEvent* resp;
};

}

#endif

