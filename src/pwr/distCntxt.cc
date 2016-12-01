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

#include "pwr_config.h"

#if HAVE_PYTHON
#include "pyConfig.h"
#endif
#if HAVE_HWLOC
#include "hwlocConfig.h"
#endif
#include <stdlib.h>
#include <string>
#include <assert.h>
#include <sys/utsname.h>

#include "distCntxt.h"
#include "distRequest.h"
#include "distObject.h"
#include "distGroup.h"
#include "distComm.h"

#include "attrInfo.h"
#include "debug.h"
#include "group.h"
#include "xmlConfig.h"
#include "util.h"
#include "device.h"

#include "tcpEventChannel.h"
#include "allocEvent.h"
#include "communicator.h"

using namespace PowerAPI;

static const char* getenv2( std::string prefix, std::string sufix ) 
{
	const char* env = getenv( (prefix + sufix).c_str() ); 
	DBG("prefix='%s' sufix='%s'\n",prefix.c_str(),sufix.c_str());

	if ( ! env ) {
		env = getenv( sufix.c_str() );
	}
	return env; 
}

DistCntxt::DistCntxt( PWR_CntxtType type, PWR_Role role, const char* name ) :
	m_name(name)
{
	DBGX("name=%s\n",name);
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
        throw -1;
    }
	
    DBGX("configFile=`%s`\n",configFile.c_str());

    size_t pos = configFile.find_last_of( "." );
    if ( std::string::npos == pos ) {
        assert(0);
    }
    if ( 0 == configFile.compare(pos,4,".xml") ) {
        m_config = new XmlConfig( configFile );
#if HAVE_PYTHON
    } else if ( 0 == configFile.compare(pos,3,".py") ) {
        m_config = new PyConfig( configFile );
#endif
#if HAVE_HWLOC
    } else if ( 0 == configFile.compare(pos,6,".hwloc") ) {
        m_config = new HwlocConfig( configFile );
#endif
    } else {
		fprintf(stderr,"ERROR: config file name format error `%s`\n",
									configFile.c_str());
		exit(-1);
	}

#if 0
    m_config->print( std::cout );
#endif

    initPlugins( *m_config );

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
DistCntxt::~DistCntxt() 
{
	delete m_evChan;
	delete m_config;
	while ( ! m_objMap.empty() ) { 
		delete m_objMap.begin()->second;
		m_objMap.erase( m_objMap.begin() );
	}

	while ( ! m_deviceMap.empty() ) {

		std::map< std::string, Device* >& tmp = m_deviceMap.begin()->second;	
		while ( ! tmp.empty() ) {
			delete tmp.begin()->second;	
			tmp.erase( tmp.begin() );
		}

		m_deviceMap.erase( m_deviceMap.begin() );
	}

	while ( ! m_devMap.empty() ) {

		m_devMap.begin()->second.first->final( m_devMap.begin()->second.second );
		m_devMap.erase( m_devMap.begin() );	
	}
	
	while ( ! m_pluginLibMap.empty() ) {
		m_pluginLibMap.erase( m_pluginLibMap.begin() );
	}
	
	while ( ! m_commMap.empty() ) {
		delete m_commMap.begin()->second;
		m_commMap.erase( m_commMap.begin() );
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

Communicator* DistCntxt::getCommunicator( std::set<std::string> objects )
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

Grp* DistCntxt::createGrp( std::string name )
{
    return new DistGrp( this, name );
}

int DistCntxt::destroyGrp( Grp* grp )
{
	delete grp;
	return PWR_RET_SUCCESS; 
}

static void sumOp( void* out, void* in, size_t num )
{
	double tmp = 0;
	for ( unsigned i = 0; i < num; i++) {
		DBG("%f\n",((double*)in)[i]);
		tmp += ((double*)in)[i];
	}
	*(double*)out = tmp;
}

static void avgOp( void* out, void* in, size_t num )
{
	double tmp = 0;
	for ( unsigned i = 0; i < num; i++) {
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
	ValueOp vOp = NO_OP;
    DBGX("obj=`%s` attr=%s\n",obj->name().c_str(),attrNameToString(attrName));

    std::string op = m_config->findAttrOp( obj->name(),attrName );
    std::string type = m_config->findAttrType( obj->name(),attrName );
    AttrInfo::OpFuncPtr opFunc = NULL;
    if ( ! op.compare("SUM") ) {
        opFunc = sumOp;
		if ( ! type.compare("Float") ) {
			vOp = FP_ADD;
		} else if ( ! type.compare("Integer") ) {
			vOp = INT_ADD;
		} else {
            assert(0);
        }

    } else if ( ! op.compare("AVG") ) {
        opFunc = avgOp;

		if ( ! type.compare("Float") ) {
			vOp = FP_AVG;
		} else if ( ! type.compare("Integer") ) {
			vOp = INT_AVG;
		} else {
            assert(0);
        }
    }

	AttrInfo* attrInfo = new AttrInfo( opFunc, timeOp, vOp );

	if ( vOp != NO_OP ) {
   		std::set<std::string> remote;
		traverse( obj->name(), attrName, attrInfo->devices, remote );

		DBGX("obj='%s' attr=`%s` op=%s type=%s\n",
						obj->name().c_str(),attrNameToString(attrName),
						op.c_str(),type.c_str());
		DBGX("local devices %lu, remote devices %lu\n",
						attrInfo->devices.size(), remote.size() );
		if ( ! remote.empty() ) {
			attrInfo->comm = getCommunicator( remote );
		}
	} else {
		DBGX("obj='%s' attr=`%s` invalid\n",
						obj->name().c_str(),attrNameToString(attrName));
	}

	return attrInfo;
}
void DistCntxt::traverse( std::string objName, PWR_AttrName attrName,
                std::vector<Device*>& local, std::set<std::string>& remote )
{
	DBGX("obj='%s' attr=`%s`\n",objName.c_str(),attrNameToString(attrName));

	if ( m_config->hasServer( objName ) && objName.compare( m_rootName ) ) {
		remote.insert( objName );
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

        if ( m_devMap.find( dev.device ) == m_devMap.end() ) {
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

// Fix me
    std::deque< Config::Plugin > plugins = m_config->findPlugins();
    std::deque< Config::Plugin >::iterator iter = plugins.begin();
	DBGX("%lu\n",plugins.size());

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
