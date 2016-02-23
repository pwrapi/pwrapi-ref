#include <stdlib.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "server.h"

#include "../router/routerEvent.h"
#include "allocEvent.h"
#include "debug.h"

using namespace PWR_Server;

static void initArgs( int argc, char* argv[], Args* args );

Server::Server( int argc, char* argv[] ) 
{
	initArgs( argc, argv, &m_args );

	setenv( (m_args.name + "POWERAPI_CONFIG" ).c_str(), 
				m_args.pwrApiConfig.c_str(), 0 );  
	setenv( (m_args.name + "POWERAPI_ROOT").c_str(), 
				m_args.pwrApiRoot.c_str(), 0 );  

	DBGX("name=%s\n",m_args.name.c_str());
	DBGX("config=%s\n",m_args.pwrApiConfig.c_str());
	DBGX("root=%s\n",m_args.pwrApiRoot.c_str());
	
	if ( ! m_args.pwrApiServer.empty() ) {
		setenv( (m_args.name + "POWERAPI_SERVER").c_str(),
				m_args.pwrApiServer.c_str(), 0 );  
	}
	if ( ! m_args.pwrApiServerPort.empty() ) {
		setenv( (m_args.name + "POWERAPI_SERVER_PORT").c_str(),
				m_args.pwrApiServerPort.c_str(), 0 );  
	}

	PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_ADMIN , m_args.name.c_str(), &m_ctx );
	assert(m_ctx);

	EventChannel* ctxChan = PWR_CntxtGetEventChannel( m_ctx );
    EventChannel* rtrChan = getEventChannel( "TCP", allocRtrEvent, 
			"server=" + m_args.host + " serverPort=" + m_args.port, "router" );

	m_chanSelect = getChannelSelect("TCP");
	assert( m_chanSelect );
	if ( ctxChan ) {
    	m_chanSelect->addChannel( ctxChan, new CntxtData( ctxChan ) ); 
	}
    m_chanSelect->addChannel( rtrChan, new RouterData( rtrChan ) );
    
	ServerConnectEvent* ev = new ServerConnectEvent;	
	ev->name = m_args.pwrApiRoot;
	rtrChan->sendEvent(ev);
}

Server::~Server() {
    PWR_CntxtDestroy( m_ctx );
}

int Server::work()
{
    SelectData* data;

    while ( ( data = static_cast<SelectData*>( m_chanSelect->wait() ) ) ) {
		DBGX("something happened on channel: %s\n",m_args.pwrApiRoot.c_str());

		if ( data->process( this ) ) {
			delete data;
			DBGX("channel broken\n");
			break;
        }
    }
	return 0;
}

void Server::initFini( Event* key, Event* x, EventChannel* y )
{
	DBGX("\n");
	m_finiMap[ key ] = std::make_pair(x,y);
}

void Server::freeFini( Event* key )
{
	DBGX("\n");
	assert( m_finiMap.find(key) != m_finiMap.end() );
	m_finiMap.erase(key);
}

void Server::fini( Event* key, Event* payload )
{
	DBGX("\n");
	assert( m_finiMap.find(key) != m_finiMap.end() );
	RouterEvent* re = static_cast<RouterEvent*>(m_finiMap[key].first);
	EventChannel* ec = static_cast<EventChannel*>(m_finiMap[key].second);

	DBGX("src=%" PRIx64 " dest=%" PRIx64"\n",re->dest,re->src);
    RouterEvent* ev = new RouterEvent( re->dest, re->src, payload );
    ec->sendEvent( ev );

	delete ev;
	delete re;
	delete key;
	m_finiMap.erase(key);
}


#include <getopt.h>
static void print_usage() {
    printf("Server::%s()\n",__func__);
}

static void initArgs( int argc, char* argv[], Args* args )
{
    int opt = 0;
    int long_index = 0;
    enum { RTR_PORT, RTR_HOST, TOP_OBJ, 
			PWRAPI_CONFIG, PWRAPI_ROOT,
			PWRAPI_SERVER, PWRAPI_SERVER_PORT, NAME  };
    static struct option long_options[] = {
        {"name"    			, required_argument, NULL, NAME },
        {"rtrPort"    		, required_argument, NULL, RTR_PORT },
        {"rtrHost"      	, required_argument, NULL, RTR_HOST },
        {"pwrApiConfig" 	, required_argument, NULL, PWRAPI_CONFIG },
        {"pwrApiRoot"   	, required_argument, NULL, PWRAPI_ROOT },
        {"pwrApiServer" 	, required_argument, NULL, PWRAPI_SERVER },
        {"pwrApiServerPort" , required_argument, NULL, PWRAPI_SERVER_PORT },
        {0,0,0,0}
    };

	optind=1;

    while ( ( opt = getopt_long( argc, argv, "", long_options, &long_index ) ) != -1 ) {
        switch(opt) {
          case NAME:
            args->name = optarg;
            break;
          case RTR_PORT:
            args->port = optarg;
            break;
          case RTR_HOST:
            args->host = optarg;
            break;
          case PWRAPI_CONFIG:
			args->pwrApiConfig = optarg;
            break;
          case PWRAPI_ROOT:
			args->pwrApiRoot = optarg;
            break;
          case PWRAPI_SERVER:
			args->pwrApiServer = optarg;
            break;
          case PWRAPI_SERVER_PORT:
			args->pwrApiServerPort = optarg;
            break;
          default: 
			print_usage();
        }
    }

    if ( args->port.empty() || args->host.empty() ) { 
        print_usage();
        exit(-1);
    }
}
