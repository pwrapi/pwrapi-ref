
#include <iostream>
#include <dlfcn.h>
#include <assert.h>

#include "devTreeNode.h"
#include "objTreeNode.h"
#include "dev.h"
#include "group.h"
#include "xmlConfig.h"
#include "cntxt.h"
#include "debug.h"

#include "ops.h"

Cntxt::Cntxt( PWR_CntxtType type, PWR_Role role, const char* name  ) :
            m_top( NULL )
{
    DBGX("enter\n");
    // find data base
    // verify type
    // verify role
    // who am I ?
    if( getenv( "POWERAPI_CONFIG" ) != NULL ) {
        m_configFile = getenv( "POWERAPI_CONFIG" );
    } else {
        m_configFile = "systemX.xml";
    }

	std::string selfName;
    if( getenv( "POWERAPI_ROOT" ) != NULL ) {
        selfName = getenv( "POWERAPI_ROOT" );
    } else {
        selfName = "plat.cab0.board0.node0";
    }

	m_config = new XmlConfig( m_configFile );
	
#if 0
	m_config->print( std::cout );
#endif

	m_top = findNode( selfName );

	initDevices( *m_config );

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

    return static_cast<ObjTreeNode*>(m_top);
}

ObjTreeNode* Cntxt::findNode( std::string name ) 
{
    DBGX("find %s\n",name.c_str());
	ObjTreeNode* node = NULL; 
	if ( m_objTreeNodeMap.find( name ) == m_objTreeNodeMap.end()) {
    	DBGX("not in table %s\n",name.c_str());
		if ( m_config->findObject( name ) ) {
    		DBGX("found in config %s\n",name.c_str());
			node = new ObjTreeNode( this, name, m_config->objType( name ) );
			static_cast<ObjTreeNode*>(node)->type();
			m_objTreeNodeMap[ name ] = node; 
		}
	} else {
		node = static_cast<ObjTreeNode*>( m_objTreeNodeMap[ name ] );
	}	
	return node;
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

			if ( m_objTreeNodeMap.find( *iter ) == m_objTreeNodeMap.end()) {
				DBGX("create %s\n",iter->c_str());
				m_objTreeNodeMap[ *iter ] = new ObjTreeNode( this, *iter, 
										m_config->objType( *iter ), node );
			}
			assert( m_objTreeNodeMap[ *iter ] );
			attr.addSrc( m_objTreeNodeMap[ *iter ] );	
		}
	}
	{
		std::deque< Config::ObjDev > devices =
			m_config->findObjDevs( node->name(), attr.name() );

		std::deque< Config::ObjDev >:: iterator iter = devices.begin();
			
		for ( ; iter != devices.end(); ++iter ) {
			Config::ObjDev& dev = *iter;

			DBGX("dev=`%s` openString=`%s`\n",dev.device.c_str(),
												 dev.openString.c_str());

			assert ( m_devMap.find( dev.device.c_str() ) != m_devMap.end() );

			DevTreeNode* node =
    		new DevTreeNode( m_devMap[dev.device].dev, 
            		m_pluginLibMap[ m_devMap[dev.device].pluginName ], 
					dev.openString );


			attr.addSrc( node );

			m_devTreeNodes.push_back( node );
		}
	}

	if ( ! attr.nodes().empty() ) {
		std::string op = m_config->findAttrOp( node->name(), attr.name() );
		DBGX("%s\n",op.c_str());
		if ( ! op.compare( "SUM" ) ) {
			attr.setOp(fpSum);
			attr.setOp2(fpSum2);
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

        void* ptr = dlopen( plugin.lib.c_str(), RTLD_LAZY);
        assert(ptr);

        void* funcPtr = dlsym(ptr,"getDev");
        assert(funcPtr);

        m_pluginLibMap[ plugin.name ] = 
                    ( (plugin_dev_t* (*)(void)) funcPtr)();
        assert( m_pluginLibMap[ plugin.name ] );
    }
}

void Cntxt::initDevices( Config& cfg )
{
	initPlugins( cfg );	

	std::deque< Config::SysDev > devices = cfg.findSysDevs(); 
	std::deque< Config::SysDev >::iterator iter = devices.begin();

	for ( ; iter != devices.end(); ++iter ) {

		Config::SysDev dev = *iter;
        DBGX("device name=`%s` plugin=`%s` initString=`%s`\n", 
			dev.name.c_str(), dev.plugin.c_str(), dev.initString.c_str() ); 

        m_devMap[ dev.name ].dev = 
			m_pluginLibMap[ dev.plugin ]->init( dev.initString.c_str() ); 
        assert( m_devMap[ dev.name ].dev );

        m_devMap[ dev.name ].pluginName = dev.plugin;
    }
}

void Cntxt::finiPlugins()
{
}

void Cntxt::finiDevices()
{
	std::map< std::string, DevMapEntry >::iterator iter = m_devMap.begin();

	for ( ; iter != m_devMap.end(); ++ iter ) {
		m_pluginLibMap[ iter->second.pluginName ]->final( iter->second.dev );
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

		if ( m_objTreeNodeMap.find( *iter ) == m_objTreeNodeMap.end()) {
			DBGX("create %s\n",iter->c_str());
			m_objTreeNodeMap[ *iter ] = new ObjTreeNode( this, *iter,
									m_config->objType( *iter ), parent );
		}
		grp->add( static_cast<ObjTreeNode*>( m_objTreeNodeMap[ *iter ] ) );	
		
	}
    return grp;
}

Grp* Cntxt::initGrp( PWR_ObjType type ) {

    Grp* grp = new Grp( this, objTypeToString( type ) );

	std::deque< std::string > objs = m_config->findObjType( type );	

	std::deque < std::string >::iterator iter = objs.begin();

	for ( ; iter != objs.end(); ++iter ) {

		if ( m_objTreeNodeMap.find( *iter ) == m_objTreeNodeMap.end()) {
			DBGX("create %s\n",iter->c_str());

			//
			// We need to fill in the parent if it's in the map
			//
			m_objTreeNodeMap[ *iter ] = new ObjTreeNode( this, *iter,
									m_config->objType( *iter ), NULL );
		}
		grp->add( static_cast<ObjTreeNode*>( m_objTreeNodeMap[ *iter ] ) );	
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
