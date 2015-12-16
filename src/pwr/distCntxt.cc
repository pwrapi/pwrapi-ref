#include <stdlib.h>
#include <string>
#include <assert.h>
#include <sys/utsname.h>

#include "distCntxt.h"
#include "distRequest.h"
#include "distObject.h"
#include "distComm.h"

#include "attrInfo.h"
#include "debug.h"
#include "group.h"
#include "xmlConfig.h"
#include "pyConfig.h"
#include "util.h"
#include "device.h"

#include "tcpEventChannel.h"
#include "allocEvent.h"
#include "communicator.h"

using namespace PowerAPI;

static const char* getenv2( std::string prefix, std::string sufix ) 
{
	const char* env = getenv( (prefix + sufix).c_str() ); 
	DBG("%s %s\n",prefix.c_str(),sufix.c_str());

	if ( ! env ) {
		env = getenv( sufix.c_str() );
	}
	return env; 
}

DistCntxt::DistCntxt( PWR_CntxtType type, PWR_Role role, const char* name ) :
	m_name(name)
{
	DBGX("\n");
	m_evChan = initEventChannel();	

	const char* env;

	env = getenv2( name, "POWERAPI_DEBUG" );

	if ( env ) {
		_DbgFlags = atoi(env);
	}

	std::string configFile;

	env = getenv2( name, "POWERAPI_CONFIG" );
    if( env ) { 
        configFile = env;
    } else {
        printf("error: environment variable `POWERAPI_CONFIG` must be set\n");
        exit(-1);
    }
	
    DBGX("configFile=`%s`\n",configFile.c_str());

    size_t pos = configFile.find_last_of( "." );
    if ( 0 == configFile.compare(pos,4,".xml") ) {
        m_config = new XmlConfig( configFile );
    } else if ( 0 == configFile.compare(pos,3,".py") ) {
        m_config = new PyConfig( configFile );
    } else {
		assert(0);
	}

#if 0
    m_config->print( std::cout );
#endif

    env = getenv2( name, "POWERAPI_ROOT" );
    if( env  ) {
    	DBGX("root=`%s`\n", env );
		m_rootName = env;
        m_rootObj = findObject(env);
		if ( ! m_rootObj ) {
        	printf("error: `POWERAPI_ROOT=%s` is invalid\n",env);
        	exit(-1);
		}
    } else {
        printf("error: environment variable `POWERAPI_ROOT` must be set\n");
        exit(-1);
    }

}

EventChannel* DistCntxt::initEventChannel()
{
	const char* tmp = getenv2( m_name,"POWERAPI_SERVER");
	if ( NULL == tmp ) {
		return NULL;
	}

    std::string server( tmp ); 
    assert( ! server.empty() );
	
	DBGX("%s\n", server.c_str() );

	tmp = getenv2(m_name.c_str(),"POWERAPI_SERVER_PORT");

    std::string serverPort( tmp );
    assert( ! serverPort.empty() );
	DBGX("%s\n", serverPort.c_str() );

    std::string config = "server=" + server + " serverPort=" + serverPort;
	return ::getEventChannel("TCP", ctx_allocEvent, config, "PWR_Cntxt" );
}

Communicator* DistCntxt::getCommunicator( std::set<Object*> objects )
{
	DBGX("\n");
	if ( m_commMap.find( objects ) == m_commMap.end() ) {
		m_commMap[ objects ] = new DistComm( this, objects );
	}
	return m_commMap[objects];
}	

Object* DistCntxt::createObject( std::string name, PWR_ObjType type,
		                                                    Cntxt* cntxt )
{
    DBGX("obj=`%s`\n",name.c_str() );
    return new DistObject( name, type, cntxt );
}

static void sumOp( void* out, void* in, size_t num )
{
	double tmp = 0;
	for ( int i = 0; i < num; i++) {
		DBG("%f\n",((double*)in)[i]);
		tmp += ((double*)in)[i];
	}
	*(double*)out = tmp;
}

static void avgOp( void* out, void* in, size_t num )
{
	double tmp = 0;
	for ( int i = 0; i < num; i++) {
		DBG("%f\n",((double*)in)[i]);
		tmp += ((double*)in)[i];
	}
	*(double*)out = tmp/num;
}

static PWR_Time timeOp( std::vector<PWR_Time> x )
{
	return x[0];
}

AttrInfo* DistCntxt::initAttr( Object* obj, PWR_AttrName attrName )
{
	DBGX("obj='%s' attr=%s\n",obj->name().c_str(),attrNameToString(attrName));
	
	ValueOp vOp = NO_OP;

    std::string op = m_config->findAttrOp( obj->name(),attrName );
    std::string type = m_config->findAttrType( obj->name(),attrName );
    AttrInfo::OpFuncPtr opFunc = NULL;
    DBGX("op=%s type=%s\n",op.c_str(),type.c_str());
    if ( ! op.compare("SUM") ) {
        opFunc = sumOp;
		if ( ! type.compare("Float") ) {
			vOp = FP_ADD;
		} else {
			vOp = INT_ADD;
		}

    } else if ( ! op.compare("AVG") ) {
        opFunc = avgOp;

		if ( ! type.compare("Float") ) {
			vOp = FP_AVG;
		} else {
			vOp = INT_AVG;
		}
    }
    assert(vOp != NO_OP );
    assert(opFunc);

	AttrInfo* attrInfo = new AttrInfo( opFunc, timeOp, vOp );

   	std::set<Object*> remote;
	traverse( obj->name(), attrName, attrInfo->devices, remote );

	DBGX("%lu %lu\n", attrInfo->devices.size(), remote.size() );
	if ( ! remote.empty() ) {
		attrInfo->comm = getCommunicator( remote );
	}

	return attrInfo;
}
void DistCntxt::traverse( std::string objName, PWR_AttrName attrName,
                    std::vector<Device*>& local, std::set<Object*>& remote )
{
	DBGX("obj='%s' attr=%s\n",objName.c_str(),attrNameToString(attrName));

	if ( m_config->hasServer( objName ) && objName.compare( m_rootName ) ) {
    	Object* tmp = findObject( objName ); 
        assert( tmp );
		remote.insert( tmp );
		return;
	}

	std::deque< Config::ObjDev > objDev = 
			m_config->findObjDevs( objName, attrName ); 
	std::deque< Config::ObjDev >::iterator iter = objDev.begin();

	DBGX("found %lu local\n",objDev.size());
	for ( ; iter != objDev.end(); ++iter ) {
		Config::ObjDev& dev = *iter;

        DBGX("dev=`%s` openString=`%s`\n",dev.device.c_str(),
                                                 dev.openString.c_str());

        if ( m_devMap.find( dev.device.c_str() ) == m_devMap.end() ) {
            if ( ! initDevice( dev.device ) ) {
                assert(0);
            }
        }
		plugin_devops_t* ops = m_devMap[dev.device].second;

		if ( m_deviceMap.find( ops ) == m_deviceMap.end() ) {
			m_deviceMap[ops][dev.openString] = new Device( ops, dev.openString );
		} else {
			if ( m_deviceMap[ops].find(dev.openString) == 
												m_deviceMap[ops].end() ) {
				m_deviceMap[ops][dev.openString] = 
											new Device( ops, dev.openString );
			}	
		}
		DBGX("ops=%p %s Device=%p\n",ops, dev.openString.c_str(),
									m_deviceMap[ ops ] [dev.openString ]);

		local.push_back( m_deviceMap[ ops ] [dev.openString ] ); 
	}	

	std::deque< std::string > children = 
			m_config->findAttrChildren( objName, attrName); 

	std::deque< std::string >::iterator j = children.begin();

	DBGX("found %lu children\n",children.size());
	for ( ; j != children.end(); ++j ) {
		traverse( *j, attrName, local, remote );
	}
}

void DistCntxt::initPlugins( Config& cfg )
{
	struct utsname name;
	int rc = uname( &name );
	assert( 0 == rc );

	std::string os( name.sysname);


    std::deque< Config::Plugin > plugins = m_config->findPlugins();
    std::deque< Config::Plugin >::iterator iter = plugins.begin();

    for ( ; iter != plugins.end(); ++ iter ) {

        Config::Plugin& plugin = *iter;

		if ( 0 == os.compare("Darwin") ) {
			plugin.lib += ".dylib";
		} else {
			plugin.lib += ".so";
		}

        DBGX("plugin name=`%s` lib=`%s`\n", plugin.name.c_str(),
                                        plugin.lib.c_str() );

        m_pluginLibMap[ plugin.name ] = getDev( plugin.lib, plugin.name );

        assert( m_pluginLibMap[ plugin.name ] );
    }
}

bool DistCntxt::initDevice( std::string& devName )
{
    DBGX("device name=`%s`\n", devName.c_str() );
    initPlugins( *m_config );

    std::deque< Config::SysDev > devices = (*m_config).findSysDevs();
    std::deque< Config::SysDev >::iterator iter = devices.begin();

    for ( ; iter != devices.end(); ++iter ) {

        Config::SysDev dev = *iter;
        DBGX("device name=`%s` plugin=`%s` initString=`%s`\n",
            dev.name.c_str(), dev.plugin.c_str(), dev.initString.c_str() );

        if ( 0 == devName.compare( dev.name ) ) {
            m_devMap[ dev.name ].second =
                m_pluginLibMap[ dev.plugin ]->init( dev.initString.c_str() );
            assert( m_devMap[ dev.name ].second );

            m_devMap[ dev.name ].first = m_pluginLibMap[ dev.plugin ];
            return true;
        }
    }
    return false;
}

int DistCntxt::makeProgress()
{
	DBGX("\n");
    EventChannel* ec = getEventChannel();

	Event* ev = ec->getEvent();
    DistCommReq* req = static_cast<DistCommReq*>((CommReq*)ev->id);
    req->process( ev );
	if ( req->m_req ) {
		DBGX("\n");
		req->m_req->execCallback();
	}
	delete ev;
	DBGX("\n");
	return PWR_RET_SUCCESS;
}
