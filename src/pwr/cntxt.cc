/* 
 * Copyright 2014-2015 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <assert.h>

#include "devTreeNode.h"
#include "objTreeNode.h"
#ifdef USE_RPC
#include "rpcTreeNode.h"
#endif
#include "pwrdev.h"
#include "group.h"
#include "xmlConfig.h"
#include "cntxt.h"
#include "debug.h"
#include "stat.h"
#include "deviceStat.h"

#include "ops.h"

using namespace PowerAPI;

Cntxt::Cntxt( PWR_CntxtType type, PWR_Role role, const char* name  ) :
            m_top( NULL ), m_standAlone( true )
{
    DBGX("enter\n");

	// Todo
    // verify type
    // verify role

    if( getenv( "POWERAPI_CONFIG" ) != NULL ) {
        m_configFile = getenv( "POWERAPI_CONFIG" );
    } else {
		printf("error: environment variable `POWERAPI_CONFIG` must be set\n");
		exit(-1);
    }
    DBGX("configFile=`%s`\n",m_configFile.c_str());

    if( getenv( "POWERAPI_LOCATION" ) != NULL ) {
        m_myLocation = getenv( "POWERAPI_LOCATION" );
    }
    DBGX("location=`%s`\n",m_myLocation.c_str());

    _DbgFlags = 0x1;
	m_config = new XmlConfig( m_configFile );

#if 0
	m_config->print( std::cout );
#endif

	std::string selfName;
    if( getenv( "POWERAPI_ROOT" ) != NULL ) {
        selfName = getenv( "POWERAPI_ROOT" );
    } else {
		printf("error: environment variable `POWERAPI_ROOT` must be set\n");
		exit(-1);
    }

    DBGX("root=`%s`\n",selfName.c_str());

    if ( getenv( "POWERAPI_STANDALONE" ) != NULL ) {
        if ( ! strcmp( getenv( "POWERAPI_STANDALONE" ), "no" ) ) {
            m_standAlone = false;
        }  
    }

    DBGX("location=`%s`\n",m_myLocation.c_str());
    DBGX("root=`%s`\n",selfName.c_str());
    DBGX("standAlone=%s\n", standAlone() ? "yes":"no");

	m_top = findNode( selfName );
    assert(m_top);
    DBGX("%s\n",m_top->name().c_str());

    DBGX("return\n");
}

Cntxt::~Cntxt()
{
	finiDevices();

    std::map<std::string, TreeNode*>::iterator iter = m_objTreeNodeMap.begin();
    for ( ; iter != m_objTreeNodeMap.end(); ++iter ) {
        delete iter->second;
    }

	delete m_config;
}

ObjTreeNode* Cntxt::getSelf() 
{
    DBGX("%s\n",m_top->name().c_str());

    return static_cast< ObjTreeNode* >(m_top);
}

ObjTreeNode* Cntxt::findNode( std::string name ) 
{
    DBGX("find %s\n",name.c_str());
    return findObject( name );
}

void Cntxt::initAttr( TreeNode* _node, TreeNode::AttrEntry& attr ) 
{
	ObjTreeNode* node = static_cast<ObjTreeNode*>(_node);
	DBGX("%s %s\n",node->name().c_str(), attrNameToString(attr.name()));
	{
		std::deque< std::string > children =
			m_config->findAttrChildren( node->name(), attr.name() );
			
		std::deque< std::string >::iterator iter = children.begin();

		for ( ; iter != children.end(); ++iter ) {

			DBGX("%s\n",iter->c_str());
			attr.addSrc( findObject( *iter ) );	
		}
	}

    DBGX("Add devs\n");

	{
		std::deque< Config::ObjDev > devices =
			m_config->findObjDevs( node->name(), attr.name() );

		std::deque< Config::ObjDev >:: iterator iter = devices.begin();
			
		for ( ; iter != devices.end(); ++iter ) {
			Config::ObjDev& dev = *iter;

			DBGX("dev=`%s` openString=`%s`\n",dev.device.c_str(),
												 dev.openString.c_str());

			if ( m_devMap.find( dev.device.c_str() ) == m_devMap.end() ) {
                if ( ! initDevice( dev.device ) ) {
                    assert(0); 
                }
            }

			DevTreeNode* node = 
				new DevTreeNode( m_devMap[dev.device].second, dev.openString );

			attr.addSrc( node );

			m_devTreeNodes.push_back( node );
		}
	}

	if ( ! attr.nodes().empty() ) {
		std::string op = m_config->findAttrOp( node->name(), attr.name() );
		DBGX("%s\n",op.c_str());
		if ( ! op.compare( "SUM" ) ) {
			attr.setOp(fpSum);
			attr.setOp(fpSum2);
		} else if ( ! op.compare( "AVG" ) ) {
			attr.setOp(fpAvg);
			attr.setOp(fpAvg2);
		} else {
			assert(0);
		}
	}
}

void Cntxt::initPlugins( Config& cfg )
{
	std::deque< Config::Plugin > plugins = m_config->findPlugins(); 
	
	std::deque< Config::Plugin >::iterator iter = plugins.begin();

    for ( ; iter != plugins.end(); ++ iter ) {
		
		Config::Plugin& plugin = *iter;

        DBGX("plugin name=`%s` lib=`%s`\n", plugin.name.c_str(),
										plugin.lib.c_str() );

        m_pluginLibMap[ plugin.name ] = getDev( plugin.lib, plugin.name );

        assert( m_pluginLibMap[ plugin.name ] );
    }
}

void Cntxt::finiPlugins()
{
}

bool Cntxt::initDevice( std::string& devName )
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

void Cntxt::finiDevices()
{
	std::map< std::string, std::pair< plugin_dev_t*, plugin_devops_t* > >
                    ::iterator iter = m_devMap.begin();

	for ( ; iter != m_devMap.end(); ++ iter ) {
		iter->second.first->final( iter->second.second );
	}
	finiPlugins();
}

Grp* Cntxt::findChildren( ObjTreeNode* parent )
{
    Grp* grp = new Grp( this, "" );
	
	std::deque< std::string > children =
			m_config->findChildren( parent->name() );
			
	std::deque< std::string >::iterator iter = children.begin();

	for ( ; iter != children.end(); ++iter ) {
		grp->add( findObject( *iter ) );	
	}
    return grp;
}

ObjTreeNode* Cntxt::findObject( std::string name )
{
    if ( m_objTreeNodeMap.find( name ) == m_objTreeNodeMap.end()) {
        TreeNode* node = NULL;

        std::string location = m_config->findObjLocation( name );
        std::string parentName = m_config->findParent( name ); 

		DBGX("create `%s` location `%s` parent `%s`\n", name.c_str(), 
                                location.c_str(), parentName.c_str());

        ObjTreeNode* parent = NULL;
        if ( m_objTreeNodeMap.find( parentName ) == m_objTreeNodeMap.end()) {
            m_objTreeNodeMap[ parentName ]; 
        }

        if ( standAlone() || 0 == location.compare( m_myLocation ) ) {
            node = new ObjTreeNode( this, name, 
                                    m_config->objType( name ), parent );
#ifdef USE_RPC
        } else {
            Config::Location tmp = m_config->findLocation( location );

            DBGX("%s %s\n", tmp.type.c_str(), tmp.config.c_str());

            node = new RpcTreeNode( this, name,
                        m_config->objType( name ), tmp.config.c_str(), parent );
#endif
        }
		assert(node);
        m_objTreeNodeMap[ name ] = node; 
	}
    return  static_cast<ObjTreeNode*>(m_objTreeNodeMap[ name ]);
}

Grp* Cntxt::initGrp( PWR_ObjType type ) {

    Grp* grp = new Grp( this, objTypeToString( type ) );

	std::deque< std::string > objs = m_config->findObjType( type );	

	std::deque < std::string >::iterator iter = objs.begin();

	for ( ; iter != objs.end(); ++iter ) {
		grp->add( findObject( *iter ) );	
	}	

    return grp;
}    

Grp* Cntxt::groupCreate( std::string name ) {
    if ( m_groupMap.find( name ) != m_groupMap.end() ) {
        return NULL;
    }
    Grp* grp = new Grp( this, name );
    m_groupMap[name] = grp;
    return grp;
}

int Cntxt::groupDestroy( Grp* grp ) {
    int retval = PWR_RET_FAILURE;
    std::map<std::string,Grp*>::iterator iter = m_groupMap.begin();
    for ( ; iter != m_groupMap.end(); ++iter ) {
        if ( iter->second == grp ) {
            delete grp;
            m_groupMap.erase( iter );
            retval = PWR_RET_SUCCESS;
            break;
        }
    }
    return retval;
}

Stat* Cntxt::createStat(ObjTreeNode* obj, PWR_AttrName name, PWR_AttrStat stat)
{
	return new DeviceStat( this, obj, name, stat );   
}

Stat* Cntxt::createStat( Grp* grp, PWR_AttrName name, PWR_AttrStat stat )
{
	return new DeviceStat( this, grp, name, stat );   
}
int Cntxt::destroyStat( Stat* stat )
{
	delete stat;
	return PWR_RET_SUCCESS;
}
