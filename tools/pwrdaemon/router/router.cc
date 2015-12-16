
#include <string>
#include <fstream>
#include <debug.h>
#include "router.h"
#include "xmlConfig.h"
#include "pyConfig.h"
#include "allocEvent.h"
#include "routerEvent.h"
#include "routerCore.h"
#include "torusCore.h"
#include "treeCore.h"

using namespace PWR_Router;

static void initArgs( int argc, char* argv[], Args* );

Router::Router( int argc, char* argv[] ) :
    m_client( this, &Router::addClientChan, &Router::delClientChan ),
    m_server( this, &Router::addServerChan, &Router::delServerChan ),
    m_router( this, &Router::addRouterChan, &Router::delRouterChan ),
    m_chanSelect(NULL)
{
    if ( NULL != getenv( "POWERAPI_DEBUG" ) ) {
        _DbgFlags = atoi( getenv( "POWERAPI_DEBUG" ) );
    }
	initArgs( argc, argv, &m_args );
	DBGX("config='%s'\n",m_args.pwrApiConfig.c_str() );

	size_t pos = m_args.pwrApiConfig.find_last_of( "." );
	if ( 0 == m_args.pwrApiConfig.compare(pos,4,".xml") ) {	
		m_config = new PowerAPI::XmlConfig( m_args.pwrApiConfig );
	} else if ( 0 == m_args.pwrApiConfig.compare(pos,3,".py") ) {
		m_config = new PowerAPI::PyConfig( m_args.pwrApiConfig );
	} else {
		assert(0);
	}

	m_chanSelect = getChannelSelect( "TCP" );

	Args& args= m_args;

    DBGX("rtrId=%d\n", args.rtrId );
    DBGX("client=%s server=%s\n", args.clientPort.c_str(), 
												args.serverPort.c_str() );

	if ( ! args.clientPort.empty()  )  {
    	EventChannel* clientChan =
                getEventChannel( "TCP", allocClientEvent, 
						"listenPort=" + args.clientPort, "client-listen" );
    	m_chanSelect->addChannel( clientChan,
				new AcceptData<EventData>(clientChan, &m_client ) );
	}
	if ( ! args.serverPort.empty()  )  {
    	EventChannel* serverChan =
                getEventChannel( "TCP", allocServerEvent, 
						"listenPort=" + args.serverPort, "server-listen" );
    	m_chanSelect->addChannel( serverChan,
				new AcceptData<EventData>(serverChan, &m_server ) );
	}

    std::string XPOS_server;
    std::string XPOS_serverPort;

	if ( 0 == m_args.coreArgs->type.compare( "torus" ) ) {
		m_routerCore = new TorusCore( m_args.coreArgs, this );
	} else if ( 0 == m_args.coreArgs->type.compare( "tree" ) ) {
		m_routerCore = new TreeCore( m_args.coreArgs, this );
	}

	initRouteTable( m_args.routeTable );
}

void Router::initRouteTable( std::string file )
{		
	std::ifstream fs;
	DBGX("%s\n",file.c_str());
	fs.open( file.c_str(), std::ifstream::in  );
	if ( fs.fail() ) {
		printf("can't read route file %s\n", file.c_str());	
	}

	std::string tmpStr;	
	while( ! std::getline( fs, tmpStr, '\n' ).eof() ) {
		size_t pos1 = tmpStr.find_first_of(":");
		size_t pos2 = tmpStr.find_last_of(":");

		
		RouterID rtrID = atoi( tmpStr.substr(pos1+1, pos2-1).c_str() );
		ServerID srvrID = atoi( tmpStr.substr(pos2+1).c_str() );
		DBGX("%s %d %d\n",tmpStr.substr(0,pos1).c_str(), rtrID, srvrID );

		m_routeTable[ tmpStr.substr( 0, pos1)  ] = APP_ID( rtrID, srvrID );  
	}
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
	AppID destID = findDestApp( destObj );
	DBGX("dest=`%s` AppID=%lx\n", destObj.c_str(), destID );
	if ( -1 == destID ) {
		printf("Could not route %s, drop event\n",destObj.c_str());
		return;
	}
	sendEvent( destID, ev ); 	
}

void Router::sendEvent( AppID dest, Event* ev ) {

	ServerID srvrID = SERVER_ID( dest );
	RouterID rtrID  = RTR_ID( dest );
	EventChannel* ec = NULL; 

	DBGX("rtr=%d srvr=%d\n",rtrID, srvrID );
	if ( rtrID == m_args.rtrId ) {
		DBGX("local channel\n");
		ec = findServerChan( srvrID ); 
	} else {
		DBGX("router channel\n");
		ec = findRtrChan( rtrID );
		assert(ec);
	}

	if ( ev->type == Router2Router ) {
		RouterEvent* rev = static_cast<RouterEvent*>(ev);
		DBGX("is RouterEvent\n");

		if ( rtrID == m_args.rtrId && -1 == srvrID ) {
			Event* pev = rev->getPayload( allocServerEvent );
			DBGX("call process\n");
			if ( pev->process( this ) ) {
				delete pev;
			}
			return;
		} 

	} else {
		DBGX("rtrId%d\n",m_args.rtrId);
		AppID src = APP_ID( m_args.rtrId, -1 );
		DBGX("create RouterEvent src=%#lx dest=%#lx\n",src,dest);
		RouterEvent* rev = new RouterEvent( src, dest, ev );
		ev = rev;
	}

	ec->sendEvent( ev );
}

AppID Router::findDestApp( ObjID id ) {
	DBGX("%s\n",id.c_str());
	return findRoute( id );
}	

EventChannel* Router::findRtrChan( RouterID id ) {
	DBGX("router id %d\n",id);
	return m_routerCore->getChannel( id );
}
EventChannel* Router::findServerChan( ServerID id ) {
	DBGX("server id %d\n", id );
	if ( -1 == id  ) {
		return NULL;
	} else {
		return m_localMap[id];
	}
}

#include <getopt.h>
static void print_usage() {
    printf("%s()\n",__func__);
}

static void initTreeInfo( TreeArgs* args, std::string info );
static void initTorusInfo( TorusArgs* args, std::string info );

static void initArgs( int argc, char* argv[], Args* args )
{
    int opt = 0;
    int long_index = 0;
    enum { CLNT_PORT, SRVR_PORT, RTR_TYPE, RTR_INFO, RTR_ID, PWRAPI_CONFIG, RTR_TABLE };
    static struct option long_options[] = {
        {"clientPort"           , required_argument, NULL, CLNT_PORT },
        {"serverPort"           , required_argument, NULL, SRVR_PORT },
        {"routerType"           , required_argument, NULL, RTR_TYPE },
        {"routerInfo"           , required_argument, NULL, RTR_INFO },
        {"routerId"             , required_argument, NULL, RTR_ID },
        {"routeTable"           , required_argument, NULL, RTR_TABLE },
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
          case RTR_TABLE:
            args->routeTable = optarg;
            break;
		  case PWRAPI_CONFIG:
			args->pwrApiConfig = optarg;
			break;
          case RTR_TYPE:
			assert( ! args->coreArgs ); 
			if ( 0 == strcmp( optarg, "torus" ) ) {
				args->coreArgs = new TorusArgs;
				args->coreArgs->type = optarg;
			} else if ( 0 == strcmp( optarg, "tree" ) ) {
				args->coreArgs = new TreeArgs;
				args->coreArgs->type = optarg;
			} else {
				assert(0);
			}
			break;
          case RTR_INFO:
			assert( args->coreArgs );
			if ( 0 == args->coreArgs->type.compare( "torus" ) ) {
				initTorusInfo( static_cast<TorusArgs*>(args->coreArgs), optarg );
			} else if ( 0 == args->coreArgs->type.compare( "tree" ) ) {
				initTreeInfo( static_cast<TreeArgs*>(args->coreArgs), optarg );
			}		

            break;
          default: print_usage();
            exit(-1);
        }
    }

    if ( (RouterID) -1 == args->rtrId 
		|| args->routeTable.empty() ) {
        print_usage();
        exit(-1);
    }
}

void initTreeInfo( TreeArgs* args, std::string info )
{
    size_t pos = info.find_first_of(':');
    unsigned int link = atoi( info.substr( 0, pos ).c_str() );
	args->links.resize( link+1);
		
   	info = info.substr(pos+1);
    pos = info.find_first_of(':');
    args->links[ link ].myListenPort = info.substr( 0, pos );

    info = info.substr(pos+1);
    pos = info.find_first_of(':');
    args->links[ link ].otherHost = info.substr( 0,pos );

   	info = info.substr(pos+1);

	if ( link > 0 ) {
   		pos = info.find_first_of(':');
    	args->links[ link ].otherHostListenPort = info.substr( 0,pos );

    	info = info.substr(pos+1);
    	pos = info.find_first_of(':');
    	args->links[ link ].nidStart = info.substr( 0,pos );

    	info = info.substr(pos+1);
    	args->links[ link ].nidStop = info.substr( 0,pos +1);

	} else {
    	args->links[ link ].otherHostListenPort = info.substr( 0,pos+1 );
	}

}

void initTorusInfo( TorusArgs* args, std::string info )
{
    size_t pos = info.find_first_of(':');
    unsigned int dim = atoi( info.substr( 0, pos ).c_str() );
	args->dim.resize(dim+1);
    if ( dim < args->dim.size() ) {
    	info = info.substr(pos+1);
        pos = info.find_first_of(':');
        args->dim[ dim ].posPort = info.substr( 0, pos );
        info = info.substr(pos+1);
        pos = info.find_first_of(':');
        args->dim[ dim ].negSrvr = info.substr( 0,pos );
        info = info.substr(pos+1);
        args->dim[ dim ].negSrvrPort = info.substr( 0,pos+1 );
	}
}
