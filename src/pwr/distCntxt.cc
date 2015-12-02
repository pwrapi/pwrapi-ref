#include <stdlib.h>
#include <string>
#include <assert.h>
#include <sys/utsname.h>

#include "distCntxt.h"
#include "distObject.h"
#include "distComm.h"

#include "attrInfo.h"
#include "debug.h"
#include "group.h"
#include "xmlConfig.h"
#include "util.h"
#include "device.h"

#include "tcpEventChannel.h"
#include "allocEvent.h"

using namespace PowerAPI;

DistCntxt::DistCntxt( PWR_CntxtType type, PWR_Role role, const char* name )
{
	DBGX("\n");
	m_evChan = initEventChannel();	
	_DbgFlags = 0xf;

	std::string configFile;

    if( getenv( "POWERAPI_CONFIG" ) != NULL ) {
        configFile = getenv( "POWERAPI_CONFIG" );
    } else {
        printf("error: environment variable `POWERAPI_CONFIG` must be set\n");
        exit(-1);
    }
    DBGX("configFile=`%s`\n",configFile.c_str());
    if( getenv( "POWERAPI_LOCATION" ) != NULL ) {
   		m_myLocation = getenv( "POWERAPI_LOCATION" );
	}
	DBGX("location=`%s`\n",m_myLocation.c_str());

    m_config = new XmlConfig( configFile );

#if 0
    m_config->print( std::cout );
#endif

    if( getenv( "POWERAPI_ROOT" ) != NULL ) {
    	DBGX("root=`%s`\n",getenv( "POWERAPI_ROOT" ) );
        m_rootObj = findObject( getenv( "POWERAPI_ROOT" ) );
    } else {
        printf("error: environment variable `POWERAPI_ROOT` must be set\n");
        exit(-1);
    }

}

EventChannel* DistCntxt::initEventChannel()
{
	char* tmp = getenv("POWERAPI_SERVER");
	if ( NULL == tmp ) {
		return NULL;
	}

    std::string server( tmp ); 
    assert( ! server.empty() );
	
	DBGX("%s\n", server.c_str() );

	tmp = getenv("POWERAPI_SERVER_PORT");

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

static void fpOp( void* inout, void* in )
{
	DBG("%f %f\n",*(double*)inout,*(double*)in);
	*(double*)inout += *(double*) in;
}

PWR_Time timeOp( PWR_Time in1, PWR_Time in2 )
{
	return in1 < in2 ? in2 : in1;
}

AttrInfo* DistCntxt::initAttr( Object* obj, PWR_AttrName attrName )
{
	DBGX("obj='%s' attr=%s\n",obj->name().c_str(),attrNameToString(attrName));
	AttrInfo* attrInfo = new AttrInfo( fpOp, timeOp );

   	std::set<Object*> remote;
	traverse( obj->name(), attrName, attrInfo->devices, remote );

	DBGX("%lu %lu\n", attrInfo->devices.size(), remote.size() );
	if ( ! remote.empty() ) {
		attrInfo->comm = getCommunicator( remote );
	}

	return attrInfo;
}
void DistCntxt::traverse( std::string objName, PWR_AttrName attrName,
                    std::deque<Device*>& local, std::set<Object*>& remote )
{
	DBGX("obj='%s' attr=%s\n",objName.c_str(),attrNameToString(attrName));

	std::deque< Config::ObjDev > objDev = 
			m_config->findObjDevs( objName, attrName ); 
	std::deque< Config::ObjDev >::iterator iter = objDev.begin();

	DBGX("found %lu local\n",objDev.size());
	for ( ; iter != objDev.end(); ++iter ) {
		Config::ObjDev& dev = *iter;
		std::string location = m_config->findObjLocation( objName );
		if ( location.compare( m_myLocation ) ) {
			DBGX("not my device\n");
			remote.insert( findObject( objName ) );
			continue;
		}

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
		std::string objLocation = m_config->findObjLocation( *j );
		DBGX("myloc=%s objLoc=%s\n",m_myLocation.c_str(), objLocation.c_str());
		if ( 0 == objLocation.compare( m_myLocation ) ) {
			traverse( *j, attrName, local, remote );
		} else {
			DBGX("obj=`%s` location=`%s`\n",(*j).c_str(), objLocation.c_str() );
			remote.insert( findObject( *j ) );
		}
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
	//pwr_CommMakeProgress( ctx );
	return PWR_RET_FAILURE;
}
