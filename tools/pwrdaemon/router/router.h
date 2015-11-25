
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

#define RTR_ID( x ) x >> 32 
#define SERVER_ID( x ) x & 0xffffffff
#define APP_ID( rtr, server ) (AppID) rtr << 32 | server

class EventChannel;
class Config;

namespace PWR_Router {

typedef uint32_t RouterID;
typedef uint32_t ServerID;

struct Dim {
    std::string posPort;
    std::string negSrvr;
    std::string negSrvrPort;
};
struct Args {
    Args( size_t numDim ) : rtrId(-1), dim(numDim) { }
    RouterID   	rtrId;
    std::string	serverPort;
    std::string clientPort;
	std::string pwrApiConfig;

    std::vector< Dim > dim;
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
		DBGX("%s\n",name.c_str());
		//assert( m_localMap.find(name) == m_localMap.end() );
		m_localMap[ 0 ] = ec;
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

  private:

	EventChannel* findRtrChan( RouterID );
	EventChannel* findServerChan( ServerID );

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

  private:
	enum { NEG_LINK, POS_LINK };
	PowerAPI::Config*               m_config;
	ChannelSelect* 			 		m_chanSelect;

	std::map<EventChannel*,Server*>	m_serverMap;
	std::map<EventChannel*,Client*>	m_clientMap;

	std::map<ServerID,EventChannel*> m_localMap;
	std::map<RouterID,EventChannel*> m_routeMap;
	std::vector< std::vector<EventChannel*> > 	m_rtrLinks;
};

struct CommReqInfo {
   	EventChannel*   src;
    CommEvent*      ev;
    EventId         id;
};

}

#endif

