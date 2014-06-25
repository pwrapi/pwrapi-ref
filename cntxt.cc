
#include <dlfcn.h>
#include <assert.h>

#include "dev.h"
#include "group.h"
#include "objectEl.h"

#ifdef USE_ULXMLRPC
#include "objectUrl.h"
#endif

#include "cntxt.h"
#include "debug.h"

using namespace tinyxml2;


_Cntxt::_Cntxt( PWR_CntxtType type, PWR_Role role, const char* name  ) :
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

    if( getenv( "POWERAPI_ROOT" ) != NULL ) {
        m_topName = getenv( "POWERAPI_ROOT" );
    } else {
        m_topName = "plat.cab0.board0.node0";
    }

    m_xml = new tinyxml2::XMLDocument();
    m_xml->LoadFile( m_configFile.c_str() );
    assert( tinyxml2::XML_SUCCESS == m_xml->ErrorID() );
    //printTree( m_xml->RootElement() );
    initPlugins( m_xml->RootElement() );
    initDevices( m_xml->RootElement() );
    DBGX("return\n");
}

_Cntxt::~_Cntxt()
{
	finiDevices();
    std::map<std::string, _Obj*>::iterator iter = m_objMap.begin();
    for ( ; iter != m_objMap.end(); ++iter ) {
        delete iter->second;
    }
}

typedef plugin_dev_t* (*funcPtr_t)(void);

void _Cntxt::initPlugins( XMLElement* el )
{
    DBGX("%s\n",el->Name());

    XMLNode* tmp = el->FirstChild(); 

    // find the children element
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        //DBGX("%s\n",el->Name());

        if ( 0 == strcmp( el->Name(), "Plugins") ) {
            tmp = el->FirstChild();
            break;
        }
        tmp = tmp->NextSibling();
    }

    // iterate over the children
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        DBGX("plugin name=`%s` lib=`%s`\n", el->Attribute("name"), el->Attribute("lib") );

        void* ptr = dlopen( el->Attribute("lib"), RTLD_LAZY);
        assert(ptr);

        void* funcPtr = dlsym(ptr,"getDev");
        assert(funcPtr);

        m_pluginLibMap[ el->Attribute("name") ] = 
                    ( (plugin_dev_t* (*)(void)) funcPtr)();
        assert( m_pluginLibMap[ el->Attribute("name") ] );

        tmp = tmp->NextSibling();
    }
}
void _Cntxt::initDevices( XMLElement* el )
{
    DBGX("%s\n",el->Name());

    XMLNode* tmp = el->FirstChild(); 

    // find the children element
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        //DBGX("%s\n",el->Name());

        if ( 0 == strcmp( el->Name(), "Devices") ) {
            tmp = el->FirstChild();
            break;
        }
        tmp = tmp->NextSibling();
    }

    // iterate over the children
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        const char* name = el->Attribute("name");
        const char* plugin = el->Attribute("plugin");
        const char* initString = el->Attribute("initString");
        DBGX("device name=`%s` plugin=`%s` initString=`%s`\n", name, plugin, initString ); 

        m_devMap[ name ].dev = m_pluginLibMap[ plugin ]->init( initString ); 
        assert( m_devMap[ el->Attribute("name") ].dev );

        m_devMap[ name ].pluginName = plugin;

        tmp = tmp->NextSibling();
    }
}

void _Cntxt::finiDevices()
{
	std::map<std::string,Y>::iterator iter = m_devMap.begin();

	for ( ; iter != m_devMap.end(); ++ iter ) {
		m_pluginLibMap[ iter->second.pluginName ]->final( iter->second.dev );
	}
}


_Dev* _Cntxt::newDev( const std::string name, const std::string config )
{
    DBGX("name=`%s` config=`%s`\n",name.c_str(), config.c_str());

    return new _Dev( m_devMap[name].dev, 
            m_pluginLibMap[ m_devMap[name].pluginName ], config );
}

_Obj* _Cntxt::getSelf() {

    if ( m_top ) return m_top;
    DBGX("\n");
    XMLElement* el = XMLFindObject( m_topName );
    assert(el);
    DBGX("\n");

    m_top = createObj( this, NULL, el );
    m_objMap[ m_top->name() ] = m_top;
    DBGX("\n");
    return m_top;
}

_Grp* _Cntxt::findChildren( XMLElement* el, _Obj* parent )
{
    _Grp* grp = NULL;
    const std::string name = el->Attribute("name"); 

    XMLNode* tmp = el->FirstChild(); 

    // find the children element
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        //DBGX("%s\n",el->Name());

        if ( 0 == strcmp( el->Name(), "children") ) {
            tmp = el->FirstChild();
            if ( tmp ) {
                grp = new _Grp(this); 
            }
            break;
        }
        tmp = tmp->NextSibling();
    }

    // iterate over the children
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        DBGX("%s\n", el->Attribute("name"));
        std::string childName = name + "." + el->Attribute("name");

		XMLElement* child = NULL;
		std::string url; 
		if ( el->Attribute("url" ) ) {
			url = el->Attribute("url");
		} else {
        	child = XMLFindObject( childName );
        	assert( child );
		}

        _Obj* obj;
        if ( m_objMap.find( childName ) == m_objMap.end() ) {
			if ( child ) {
            	obj = createObj( this, parent, child );
#ifdef USE_ULXMLRPC
			} else {
            	obj = createObj( this, parent, childName, url );
#endif
			}
            m_objMap[ obj->name() ] = obj;
        } else {
            obj = m_objMap[ childName ];
        } 
        grp->add( obj );
        DBGX("%s done\n", el->Attribute("name"));

        tmp = tmp->NextSibling();
    }
    
    DBGX("return\n");
    return grp;
}

_Grp* _Cntxt::initGrp( PWR_ObjType type ) {
    _Grp* grp = new _Grp( this, objTypeToString( type ) );

    XMLNode* tmp = m_xml->RootElement()->FirstChild(); 

    std::string typeStr = objTypeToString( type ); 

    // find the objects element
    while ( tmp ) {
        XMLElement* el = static_cast<XMLElement*>(tmp);

        //DBGX("%s\n",el->Name());

        if ( 0 == strcmp( el->Name(), "Objects") ) {
            tmp = el->FirstChild();
            break;
        }
        tmp = tmp->NextSibling();
    }

    while ( tmp ) {
        XMLElement* el = static_cast<XMLElement*>(tmp);


        if ( 0 == typeStr.compare( el->Attribute("type") ) ) {

#if  1 
        DBGX("%s %s name=`%s`\n",el->Name(),
                el->Attribute("type"), el->Attribute("name"));
#endif
            
            _Obj* obj;
            std::string name = el->Attribute("name");
            if ( m_objMap.find( name ) == m_objMap.end() ) {
                obj = createObj( this, NULL, el );
                m_objMap[ obj->name() ] = obj;
            } else {
                obj = m_objMap[ name ];
            } 

            grp->add( obj );
        }

        tmp = tmp->NextSibling();
    }

    return grp;
}    

_Grp* _Cntxt::groupCreate( std::string name ) {
    if ( m_groupMap.find( name ) != m_groupMap.end() ) {
        return NULL;
    }
    _Grp* grp = new _Grp( this, name );
    m_groupMap[name] = grp;
    return grp;
}

int _Cntxt::groupDestroy( _Grp* grp ) {
    int retval = PWR_ERR_FAILURE;
    std::map<std::string,_Grp*>::iterator iter = m_groupMap.begin();
    for ( ; iter != m_groupMap.end(); ++iter ) {
        if ( iter->second == grp ) {
            delete grp;
            m_groupMap.erase( iter );
            retval = PWR_ERR_SUCCESS;
            break;
        }
    }
    return retval;
}


XMLElement* _Cntxt::XMLFindObject( const std::string name )
{
    DBGX("`%s`\n",name.c_str());
    XMLNode* tmp = m_xml->RootElement()->FirstChild(); 

    // find the objects element
    while ( tmp ) {
        XMLElement* el = static_cast<XMLElement*>(tmp);

        //DBGX("%s\n",el->Name());

        if ( 0 == strcmp( el->Name(), "Objects") ) {
            tmp = el->FirstChild();
            break;
        }
        tmp = tmp->NextSibling();
    }

    while ( tmp ) {
        XMLElement* el = static_cast<XMLElement*>(tmp);

#if 0 
        DBGX("%s %s name=`%s`\n",el->Name(),
                el->Attribute("type"), el->Attribute("name"));
#endif

        if ( 0 == name.compare( el->Attribute("name") ) ) {
            return el;
        }
        tmp = tmp->NextSibling();
    }

    return NULL;
}

void _Cntxt::printTree( XMLNode* node )
{
    XMLElement* el = static_cast<XMLElement*>(node);

    if ( NULL == el ) return;

    if ( ! strcmp(el->Name(),"obj") ) {
        DBGX("%s type=`%s` name=`%s`\n",el->Name(),
                el->Attribute("type"),
                el->Attribute("name"));
    } else if ( ! strcmp(el->Name(),"child") ) {
        DBGX("%s name=`%s`\n",el->Name(),el->Attribute("name"));
    } else if ( ! strcmp(el->Name(),"attr") ) {
        DBGX("%s name=`%s` op=`%s`\n",el->Name(), el->Attribute("name"),
                        el->Attribute("op"));
    } else {
       DBGX("%s \n",el->Name());
    }

    if ( ! node->NoChildren() ) {
        XMLNode* tmp = node->FirstChild();
        do {
            printTree( tmp );
        } while ( ( tmp = tmp->NextSibling() ) );
    }
}
