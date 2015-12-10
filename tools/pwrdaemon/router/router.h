
#ifndef _ROUTER_H
#define _ROUTER_H

#include <map>
#include <set>
#include "routerSelect.h"
#include "debug.h"
#include "events.h"
#include <config.h>
#include <tcpEventChannel.h>
#include "routerEvent.h"
#include "commCreateEvent.h"
#include "routerCore.h"

#define RTR_ID( x ) x >> 32 
#define SERVER_ID( x ) x & 0xffffffff
#define APP_ID( rtr, server ) (AppID) (unsigned)rtr << 32 | (unsigned)server

class EventChannel;
class Config;

namespace PWR_Router {

typedef uint32_t ServerID;

struct Args {
    Args( ) : rtrId(-1), coreArgs(NULL) { }
    RouterID   	rtrId;
	std::string routeTable;
    std::string	serverPort;
    std::string clientPort;
	std::string pwrApiConfig;

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
		std::vector<ObjID>& getCommList( CommID id );

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

	void addServer( std::string name, EventChannel* ec ) {
		AppID id = findRoute( name );

		DBGX("rootObj=`%s` rtrId=%d serverId=%d\n",
			name.c_str(), RTR_ID(id), SERVER_ID(id)  );

		assert( -1 != id );

		m_localMap[ SERVER_ID(id) ] = ec;
		m_serverMap[ ec ]->initName(name);
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

  private:

	EventChannel* findRtrChan( RouterID );
	EventChannel* findServerChan( ServerID );

	AppID findRoute( ObjID id ) {
    	AppID retval = -1;
    	if ( m_routeTable.find(id) != m_routeTable.end() ) {
        	retval = m_routeTable[id];
   		}
		DBGX("name=`%s` AppID=%lx\n", id.c_str(), retval  )
    	return retval;
	}

	void addClientChan( EventChannel* ec) {
		DBGX("\n");
		m_clientMap[ec] = new Client(*this);
	}			

	void delClientChan( EventChannel* ec ) {
		DBGX("\n");
		delete m_clientMap[ec];
		m_clientMap.erase(ec);
	}			

	void addServerChan( EventChannel* ec) {
		DBGX("\n");
		m_serverMap[ec] = new Server(*this);
	}			

	void delServerChan( EventChannel* ec ) {
		DBGX("\n");
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
	PowerAPI::Config*               m_config;
	ChannelSelect* 			 		m_chanSelect;

	std::map<EventChannel*,Server*>	m_serverMap;
	std::map<EventChannel*,Client*>	m_clientMap;

	std::map<ServerID,EventChannel*> m_localMap;
	RouterCore* 					m_routerCore;
	std::map< std::string, AppID >  m_routeTable;
};

struct CommReqInfo {
   	EventChannel*   src;
    CommEvent*      ev;
    EventId         id;
};

}

#endif

