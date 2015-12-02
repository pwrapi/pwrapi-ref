#include <stdlib.h>

#include "server.h"

#include "allocEvent.h"
#include "debug.h"

using namespace PWR_Server;

static void initArgs( int argc, char* argv[], Args* args );

Server::Server( int argc, char* argv[] ) 
{
	initArgs( argc, argv, &m_args );

	setenv( "POWERAPI_CONFIG", m_args.pwrApiConfig.c_str(), 0 );  
	setenv( "POWERAPI_ROOT", m_args.pwrApiRoot.c_str(), 0 );  

	DBGX("%s\n",m_args.pwrApiConfig.c_str());
	DBGX("%s\n",m_args.pwrApiRoot.c_str());
	
	if ( ! m_args.pwrApiLocation.empty() ) {
		setenv( "POWERAPI_LOCATION", m_args.pwrApiLocation.c_str(), 0 );  
	}
	if ( ! m_args.pwrApiServer.empty() ) {
		setenv( "POWERAPI_SERVER", m_args.pwrApiServer.c_str(), 0 );  
	}
	if ( ! m_args.pwrApiServerPort.empty() ) {
		setenv( "POWERAPI_SERVER_PORT", m_args.pwrApiServerPort.c_str(), 0 );  
	}

    m_ctx = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_ADMIN , "");
	_DbgFlags = 0x5;
	EventChannel* ctxChan = PWR_CntxtGetEventChannel( m_ctx );
    EventChannel* rtrChan = getEventChannel( "TCP", allocRtrEvent, 
			"server=" + m_args.host + " serverPort=" + m_args.port, "router" );

	m_chanSelect = getChannelSelect("TCP");
	if ( ctxChan ) {
    	m_chanSelect->addChannel( ctxChan, new CntxtData( ctxChan ) ); 
	}
    m_chanSelect->addChannel( rtrChan, new RouterData( rtrChan ) );
    
	ServerConnectEvent* ev = new ServerConnectEvent;	
	ev->name = argv[3];
	rtrChan->sendEvent(ev);
}

Server::~Server() {
    PWR_CntxtDestroy( m_ctx );
}

int Server::work()
{
    SelectData* data;

    while ( ( data = static_cast<SelectData*>( m_chanSelect->wait() ) ) ) {
		printf("something happened on channel\n");

		if ( data->process( this ) ) {
			delete data;
			printf("channel broken\n");
			break;
        }
    }
	return 0;
}

#include <getopt.h>
static void print_usage() {
    printf("%s()\n",__func__);
}

static void initArgs( int argc, char* argv[], Args* args )
{
    int opt = 0;
    int long_index = 0;
    enum { RTR_PORT, RTR_HOST, TOP_OBJ, 
			PWRAPI_CONFIG, PWRAPI_ROOT, PWRAPI_LOCATION, 
			PWRAPI_SERVER, PWRAPI_SERVER_PORT  };
    static struct option long_options[] = {
        {"rtrPort"    		, required_argument, NULL, RTR_PORT },
        {"rtrHost"      	, required_argument, NULL, RTR_HOST },
        {"topObj"       	, required_argument, NULL, TOP_OBJ },
        {"pwrApiConfig" 	, required_argument, NULL, PWRAPI_CONFIG },
        {"pwrApiRoot"   	, required_argument, NULL, PWRAPI_ROOT },
        {"pwrApiLocation"   , required_argument, NULL, PWRAPI_LOCATION },
        {"pwrApiServer" 	, required_argument, NULL, PWRAPI_SERVER },
        {"pwrApiServerPort" , required_argument, NULL, PWRAPI_SERVER_PORT },
        {0,0,0,0}
    };

	optind=1;

    while ( ( opt = getopt_long( argc, argv, "", long_options, &long_index ) ) != -1 ) {
        switch(opt) {
          case RTR_PORT:
            args->port = optarg;
            break;
          case RTR_HOST:
            args->host = optarg;
            break;
          case TOP_OBJ:
            args->topObj = optarg;
            break;
          case PWRAPI_CONFIG:
			args->pwrApiConfig = optarg;
            break;
          case PWRAPI_ROOT:
			args->pwrApiRoot = optarg;
            break;
          case PWRAPI_LOCATION:
			args->pwrApiLocation = optarg;
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

    if ( args->port.empty() ||
          args->host.empty() ||
          args->topObj.empty() ) { 

        print_usage();
        exit(-1);
    }
}
