
#include <debug.h>
#include "router.h"
#include "xmlConfig.h"
#include "allocEvent.h"
#include "routerEvent.h"
#include "routerCore.h"
#include "torusCore.h"

using namespace PWR_Router;

static void initArgs( int argc, char* argv[], Args* );

Router::Router( int argc, char* argv[] ) :
    m_client( this, &Router::addClientChan, &Router::delClientChan ),
    m_server( this, &Router::addServerChan, &Router::delServerChan ),
    m_router( this, &Router::addRouterChan, &Router::delRouterChan ),
    m_chanSelect(NULL)
{
	initArgs( argc, argv, &m_args );
	_DbgFlags = 0x5;
	printf("config='%s'\n",m_args.pwrApiConfig.c_str() );
	m_config = new PowerAPI::XmlConfig( m_args.pwrApiConfig );

	m_chanSelect = getChannelSelect( "TCP" );

	Args& args= m_args;

    printf("rtrId=%d\n", args.rtrId );
    printf("client=%s server=%s\n", args.clientPort.c_str(), 
												args.serverPort.c_str() );

    EventChannel* clientChan =
                getEventChannel( "TCP", allocClientEvent, 
						"listenPort=" + args.clientPort, "client" );
    EventChannel* serverChan =
                getEventChannel( "TCP", allocServerEvent, 
						"listenPort=" + args.serverPort, "server" );


    m_chanSelect->addChannel( clientChan,
				new AcceptData<EventData>(clientChan, &m_client ) );
    m_chanSelect->addChannel( serverChan,
				new AcceptData<EventData>(serverChan, &m_server ) );

    std::string XPOS_server;
    std::string XPOS_serverPort;

	m_routerCore = new TorusCore( m_args.coreArgs, this );
}

int Router::work()
{
	SelectData* data;

    while ( ( data = static_cast<SelectData*>( m_chanSelect->wait() ) ) ) {
        if ( data->process( m_chanSelect, this ) ) {
            delete data;
        }
    }
	return 0;
}

void Router::sendEvent( ObjID destObj, Event* ev ) {
	sendEvent( findDestApp( destObj ), ev ); 	
}

void Router::sendEvent( AppID dest, Event* ev ) {

	ServerID srvrID = SERVER_ID( dest );
	RouterID rtrID  = RTR_ID( dest );
	EventChannel* ec = NULL; 

	DBGX("rtr=%d srvr=%d\n",rtrID, srvrID );
	if ( rtrID == m_args.rtrId ) {
		DBGX("local channel\n");
		ec = findServerChan( srvrID ); 
		//ec = findServerChan( "daemon-cab0" ); 
	} else {
		DBGX("router channel\n");
		ec = findRtrChan( rtrID );
	}

	if ( ev->type == Router2Router ) {
		RouterEvent* rev = static_cast<RouterEvent*>(ev);
		DBGX("is RouterEvent\n");

		if ( rtrID == m_args.rtrId && rev->isResponse ) {
			Event* pev = rev->getPayload( allocServerEvent );
			DBGX("call process\n");
			if ( pev->process( this ) ) {
				delete pev;
			}
			return;
		} 

	} else {
		DBGX("create RouterEvent\n");
		AppID src = APP_ID( m_args.rtrId, 0 );
		RouterEvent* rev = new RouterEvent( src, dest, ev );
		ev = rev;
	}

	ec->sendEvent( ev );
}

AppID Router::findDestApp( ObjID id ) {
	DBGX("%s\n",id.c_str());
	return APP_ID( (m_args.rtrId + 1) % 2, 0 );
}	

EventChannel* Router::findRtrChan( RouterID id ) {
	DBGX("router id %d\n",id);
	return m_routerCore->getChannel( id );
}
EventChannel* Router::findServerChan( ServerID id ) {
	DBGX("server id %d\n", id );
	return m_localMap[id];
}

#include <getopt.h>
static void print_usage() {
    printf("%s()\n",__func__);
}

static void initArgs( int argc, char* argv[], Args* args )
{
    int opt = 0;
    int long_index = 0;
    enum { CLNT_PORT, SRVR_PORT, RTR_DIM, RTR_ID, PWRAPI_CONFIG };
    static struct option long_options[] = {
        {"clientPort"           , required_argument, NULL, CLNT_PORT },
        {"serverPort"           , required_argument, NULL, SRVR_PORT },
        {"routerDim"            , required_argument, NULL, RTR_DIM },
        {"routerId"             , required_argument, NULL, RTR_ID },
		{"pwrApiConfig"     	, required_argument, NULL, PWRAPI_CONFIG },
        {0,0,0,0}
    };

	optind = 1;

    while ( ( opt = getopt_long( argc, argv, "", long_options, &long_index ) ) != -1 ) {
        switch(opt) {
          case CLNT_PORT:
            args->clientPort = optarg;
            break;
          case SRVR_PORT:
            args->serverPort = optarg;
            break;
          case RTR_ID:
            args->rtrId = atoi(optarg);
            break;
		  case PWRAPI_CONFIG:
			args->pwrApiConfig = optarg;
			break;
          case RTR_DIM:
            {
				TorusArgs* cArgs =	new TorusArgs;
                std::string tmp = optarg;
                size_t pos = tmp.find_first_of(':');
                unsigned int dim = atoi( tmp.substr( 0, pos ).c_str() );
				cArgs->dim.resize(dim+1);
                if ( dim < cArgs->dim.size() ) {
                    tmp = tmp.substr(pos+1);
                    pos = tmp.find_first_of(':');
                    cArgs->dim[ dim ].posPort = tmp.substr( 0, pos );
                    tmp = tmp.substr(pos+1);
                    pos = tmp.find_first_of(':');
                    cArgs->dim[ dim ].negSrvr = tmp.substr( 0,pos );
                    tmp = tmp.substr(pos+1);
                    cArgs->dim[ dim ].negSrvrPort = tmp.substr( 0,pos+1 );
                }
				args->coreArgs = cArgs; 

            }
            break;
          default: print_usage();
            exit(-1);
        }
    }

    if ( args->clientPort.empty() ||
          args->serverPort.empty() ||
          (RouterID) -1 == args->rtrId ) {
        print_usage();
        exit(-1);
    }
}
