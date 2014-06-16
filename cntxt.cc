
#include <dlfcn.h>
#include <assert.h>

#include "./dev.h"
#include "./object.h"
#include "./group.h"
#include "./objectUrl.h"

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
    initDevices( m_xml->RootElement() );
    DBGX("return\n");
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

        DBGX("device name=`%s` lib=`%s`\n", el->Attribute("name"), el->Attribute("lib") );

        m_devLibMap[ el->Attribute("name") ] = el->Attribute("lib"); 
        tmp = tmp->NextSibling();
    }
}

typedef plugin_dev_t* (*funcPtr_t)(void);

_Dev* _Cntxt::findDev( const std::string name, const std::string config )
{
    DBGX("name=`%s` config=`%s`\n",name.c_str(), config.c_str());

    if ( m_devMap.find(name+config) == m_devMap.end() ) {

        std::map<std::string,std::string>::iterator iter;
        if ( (iter = m_devLibMap.find(name)) == m_devLibMap.end() ) {
            return NULL;
        }
        DBGX("device `%s` library `%s`\n",name.c_str(), (*iter).second.c_str());
        void* ptr = dlopen((*iter).second.c_str(),RTLD_LAZY);
        assert(ptr);
        funcPtr_t funcPtr = (funcPtr_t)dlsym(ptr,"getDev");
        assert(funcPtr);
    
        m_devMap[ name + config ] = new _Dev( funcPtr(), config ); 
    } 
    return m_devMap[name + config];
}

_Obj* _Cntxt::getSelf() {

    if ( m_top ) return m_top;
    DBGX("\n");
    XMLElement* el = XMLFindObject( m_topName );
    assert(el);
    DBGX("\n");

    m_top = new _ObjEl( this, NULL, el );
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
            	obj = new _ObjEl( this, parent, child );
			} else {
            	obj = new _ObjUrl( this, parent, childName, url );
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
                obj = new _ObjEl( this, NULL, el );
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
