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

#include <assert.h>

#include "xmlConfig.h"
#include "debug.h"

using namespace PowerAPI;

XmlConfig::XmlConfig( std::string file ) 
{
	DBGX2(DBG_CONFIG,"config file `%s`\n",file.c_str());
    m_xml = new tinyxml2::XMLDocument();
    m_xml->LoadFile( file.c_str() );
	m_systemNode = findNode( m_xml->RootElement(), "System" ); 
	assert( m_systemNode );
}

void XmlConfig::print( std::ostream& out  )
{
	printTree( out, m_xml->RootElement() );
}

void XmlConfig::printTree( std::ostream& out, XMLNode* node )
{
    XMLElement* el = static_cast<XMLElement*>(node);

    if ( NULL == el ) return;

    if ( ! strcmp(el->Name(),"obj") ) {
		out << el->Name() << " type="<< el->Attribute("type") <<
							" name "<<  el->Attribute("name") << std::endl;   
    } else if ( ! strcmp(el->Name(),"child") ) {
		out << el->Name() << " name=" << el->Attribute("name") << std::endl;
    } else if ( ! strcmp(el->Name(),"attr") ) {
		out << el->Name() << " name=" << el->Attribute("name") <<
							" op=" << el->Attribute("op") << std::endl;
    } else {
#if 0
		out << el->Name() << std::endl;
#endif
    }

    if ( ! node->NoChildren() ) {
        XMLNode* tmp = node->FirstChild();
        do {
            printTree( out, tmp );
        } while ( ( tmp = tmp->NextSibling() ) );
    }

}

bool XmlConfig::hasServer( const std::string name ) 
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());
	return ! findObjLocation( name ).empty();
}

bool XmlConfig::hasObject( const std::string name ) 
{
	DBGX2(DBG_CONFIG,"find %s\n",name.c_str());
	return ( NULL != findObject( name ) );
}

PWR_ObjType XmlConfig::objType( const std::string name )
{
	XMLElement* elm =findObject( name );
	return elm ?  getType( elm ) : PWR_OBJ_INVALID;
}

PWR_ObjType XmlConfig::getType( XMLElement* elm )
{
	return objTypeStrToInt( elm->Attribute("type") );
}

std::deque< Config::Plugin > XmlConfig::findPlugins( )
{
	std::deque< Config::Plugin > ret;
	
	XMLNode* node = findNodes1stChild( m_systemNode->FirstChild(), "Plugins" );

	while ( node ) {
        XMLElement* elm = static_cast<XMLElement*>(node);

		assert( ! strcmp( elm->Name(), "plugin" ) );

		DBGX2(DBG_CONFIG,"name=`%s` lib=`%s`\n",elm->Attribute("name"),
											elm->Attribute("lib"));

		Config::Plugin plugin;
		plugin.name = elm->Attribute("name"); 
		plugin.lib = elm->Attribute("lib");
		ret.push_back( plugin );

        node = node->NextSibling();
	}
	return ret;
}

std::deque< Config::SysDev > XmlConfig::findSysDevs()
{
	std::deque< Config::SysDev > ret;

	XMLNode* node = findNodes1stChild( m_systemNode->FirstChild(), "Devices" );

	while ( node ) {
        XMLElement* elm = static_cast<XMLElement*>(node);

		assert( ! strcmp( elm->Name(), "device" ) );

		Config::SysDev dev;
		dev.name = elm->Attribute("name");
		dev.plugin = elm->Attribute("plugin");
		dev.initString = elm->Attribute("initString");
		ret.push_back( dev );

        node = node->NextSibling();
	}

	return ret;
}

std::deque< Config::ObjDev > 
			XmlConfig::findObjDevs( std::string name, PWR_AttrName attr )
{
	DBGX2(DBG_CONFIG,"%s %s\n",name.c_str(), attrNameToString(attr).c_str());

	std::deque< Config::ObjDev > ret;

	XMLElement* obj = findObject( name );
	assert( obj );
	XMLNode* node = findAttr( obj, attrNameToString(attr) );		
	if ( ! node ) return ret;

	node = node->FirstChild();
    while ( node ) {
        XMLElement* elm = static_cast<XMLElement*>(node);

		assert( ! strcmp( elm->Name(), "src" ) );

        if ( 0 == strcmp( elm->Attribute("type"), "device") ) {
			DBGX2(DBG_CONFIG,"%s\n",elm->Attribute("name") );
			std::string tmp = elm->Attribute("name");
			XMLElement* devElm = static_cast<XMLElement*>(findDev( obj, tmp ));
			assert( devElm );

			Config::ObjDev dev;

			dev.device = devElm->Attribute("device");
			dev.openString = devElm->Attribute("openString");
			ret.push_back( dev );

			DBGX2(DBG_CONFIG,"%s %s\n", dev.device.c_str(), dev.openString.c_str() );
        }

        node = node->NextSibling();
    }

	return ret;
}

std::deque< std::string >
        XmlConfig::findAttrChildren( std::string name, PWR_AttrName attr )
{
	DBGX2(DBG_CONFIG,"%s %s\n",name.c_str(), attrNameToString(attr).c_str());

	std::deque< std::string > ret;

	XMLElement* obj = findObject( name );
	assert( obj );
	XMLNode* node = findAttr( obj, attrNameToString(attr) );		
	if ( ! node ) return ret;

	node = node->FirstChild();

    while ( node ) {
        XMLElement* elm = static_cast<XMLElement*>(node);

		assert( ! strcmp( elm->Name(), "src" ) );

        if ( 0 == strcmp( elm->Attribute("type"), "child") ) {
            DBGX2(DBG_CONFIG,"%s\n",elm->Attribute("name") );
            ret.push_back( elm->Attribute("name") );            

            DBGX2(DBG_CONFIG,"%s\n",ret.back().c_str());
        }

        node = node->NextSibling();
    }

    std::deque< std::string >::iterator iter = ret.begin(); 

    for ( ; iter != ret.end(); ++iter ) {
        
        DBGX2(DBG_CONFIG,"child's name `%s`\n",iter->c_str());

        node = findNodes1stChild( obj->FirstChild(), "children" );
        assert(node);

        while ( node ) {
            XMLElement* elm = static_cast<XMLElement*>(node);

            if ( 0 == iter->compare( elm->Attribute("name") ) ) {
                DBGX2(DBG_CONFIG,"found `%s`\n",elm->Attribute("name"));
            }

            DBGX2(DBG_CONFIG,"%s\n",iter->c_str());
            
            node = node->NextSibling();
        }
    }

    for ( iter = ret.begin() ; iter != ret.end(); ++iter ) {
        
        DBGX2(DBG_CONFIG,"child's name `%s`\n",iter->c_str());
        *iter = std::string(obj->Attribute("name")) + "." + *iter;
    }

	return ret;
}

std::string XmlConfig::findAttrOp( std::string name, PWR_AttrName attr )
{
	DBGX2(DBG_CONFIG,"%s %s\n",name.c_str(), attrNameToString(attr).c_str());

	XMLElement* obj = findObject( name );
	assert( obj );

	XMLNode* node = findAttr( obj, attrNameToString(attr) );		
	if ( NULL == node ) {
		return "";
	}else {
        const char * tmp = static_cast<XMLElement*>(node)->Attribute("op");
        return tmp ? tmp : "";
	}
}

std::string XmlConfig::findAttrHz( std::string name, PWR_AttrName attr )
{
	DBGX2(DBG_CONFIG,"%s %s\n",name.c_str(), attrNameToString(attr).c_str());

	XMLElement* obj = findObject( name );
	assert( obj );

	XMLNode* node = findAttr( obj, attrNameToString(attr) );		
	if ( NULL == node ) {
		return "";
	}else {
        const char * tmp = static_cast<XMLElement*>(node)->Attribute("hz");
        return tmp ? tmp : "";
	}
}

std::deque< std::string > XmlConfig::findChildren( std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str() );

	std::deque< std::string > ret;

	XMLElement* obj = findObject( name );
	assert( obj );

	XMLNode* node = findNodes1stChild( obj->FirstChild(), "children" );

    while ( node ) {
        XMLElement* elm = static_cast<XMLElement*>(node);

		assert( ! strcmp( elm->Name(), "child" ) );

		std::string tmp = obj->Attribute("name");
		tmp += ".";
		tmp += elm->Attribute("name");			
		ret.push_back( tmp ); 
		DBGX2(DBG_CONFIG,"found child %s\n",ret.back().c_str());

        node = node->NextSibling();
    }

	return ret;
}

std::deque< std::string > XmlConfig::findObjType( PWR_ObjType type )
{
	std::deque< std::string > ret;

	std::string typeStr = objTypeToString( type ); 
	XMLNode* node = findNodes1stChild( m_systemNode->FirstChild(), "Objects" );

	while ( node ) {
        XMLElement* elm = static_cast<XMLElement*>(node);

		assert( ! strcmp( elm->Name(), "obj" ) );

		DBGX2(DBG_CONFIG,"name=`%s` type=`%s`\n",elm->Attribute("name"),
											elm->Attribute("type"));

		if ( ! typeStr.compare( elm->Attribute("type") ) ) {
			ret.push_back( elm->Attribute("name") );
		}

        node = node->NextSibling();
	}

	return ret;
}

XMLNode* XmlConfig::findDev( XMLElement* elm, std::string name )
{
    DBGX2(DBG_CONFIG,"name=%s\n",name.c_str());
	return findNodeWithAttr( elm, "devices", "name", name );
} 

XMLNode* XmlConfig::findAttr( XMLElement* elm, std::string attr )
{
    DBGX2(DBG_CONFIG,"attr=%s\n",attr.c_str());
	return findNodeWithAttr( elm, "attributes", "name", attr );
} 

XMLNode* XmlConfig::findNodeWithAttr( XMLElement* elm, 
	std::string nodeName, std::string attrName, std::string attrValue )
{
    XMLNode* node = elm->FirstChild();

	DBGX2(DBG_CONFIG,"nodeName=%s attrName=%s attrValue=%s\n",nodeName.c_str(), 
			attrName.c_str(), attrValue.c_str());

    // find the attributes element
    while ( node ) {
        elm = static_cast<XMLElement*>(node);

        if ( 0 == nodeName.compare( elm->Name() ) ) {
            node = elm->FirstChild();
            break;
        }
        node = node->NextSibling();
    }

	while ( node ) {
        XMLElement* elm = static_cast<XMLElement*>(node);

		if ( 0 == attrValue.compare( elm->Attribute(attrName.c_str())) ) {
			DBGX2(DBG_CONFIG,"found %s\n",elm->Attribute(attrName.c_str()));
			break;
		}

        node = node->NextSibling();
	} 
			
	return node;
} 

XMLNode* XmlConfig::findNodes1stChild( XMLNode* node, const std::string name )
{
	node = findNode( node, name );
	if ( node ) {
		return node->FirstChild();
	} else {
		return NULL;
	}
}

XMLElement* XmlConfig::findObject( const std::string name )
{
    DBGX2(DBG_CONFIG,"`%s`\n",name.c_str());

	XMLNode* node = findNodes1stChild( m_systemNode->FirstChild(), "Objects" );

    while ( node ) {
        XMLElement* el = static_cast<XMLElement*>(node);

#if 0 
        DBGX2(DBG_CONFIG,"%s %s name=`%s`\n",el->Name(),
                el->Attribute("type"), el->Attribute("name"));
#endif

        if ( 0 == name.compare( el->Attribute("name") ) ) {
            return el;
        }
        node = node->NextSibling();
    }

    return NULL;
}

std::string XmlConfig::findParent( std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());
	size_t pos= name.find_last_of( "." );
	if ( pos != std::string::npos ) { 
		return name.substr( 0, pos ); 
	} else {
    	return "";
	}
}
std::string XmlConfig::findObjLocation( std::string name )
{
    XMLElement* elm = findObject( name );
    assert(elm);

    DBGX2(DBG_CONFIG,"name=%s location=`%s`\n",
                        elm->Attribute("name"),elm->Attribute("location"));

    while ( ! elm->Attribute("location") ) {
        name = findParent( name );
		if (  name.empty() ) {
			break;
		} 
		DBGX2(DBG_CONFIG,"%s\n",name.c_str());
        elm = findObject(name);
        assert(elm);
    } 
    if ( elm->Attribute("location") ) {
        return elm->Attribute("location");
    } else {
        return "";
    }  
}

XMLNode* XmlConfig::findNode( XMLNode* node, const std::string name )
{
	DBGX2(DBG_CONFIG,"root=%s find xml element `%s`\n", 
			static_cast<XMLElement*>(node)->Name(), name.c_str());

    // find the plugins element
    while ( node ) {
        XMLElement* elm = static_cast<XMLElement*>(node);

		DBGX2(DBG_CONFIG,"current=`%s`\n", elm->Name() );
        if ( 0 == name.compare( elm->Name() ) ) {
			DBGX2(DBG_CONFIG,"found element `%s`\n", name.c_str());
            break;
        }
        node = node->NextSibling();
    }
	return node; 
}

std::string XmlConfig::objTypeToString( PWR_ObjType type )
{
    switch( type ) {
    case PWR_OBJ_PLATFORM:    return "Platform";
    case PWR_OBJ_CABINET:     return "Cabinet";
    case PWR_OBJ_CHASSIS:     return "Chassis";
    case PWR_OBJ_BOARD:       return "Board";
    case PWR_OBJ_NODE:        return "Node";
    case PWR_OBJ_SOCKET:      return "Socket";
    case PWR_OBJ_POWER_PLANE: return "Power Plane";
    case PWR_OBJ_CORE:        return "Core";
    case PWR_OBJ_NIC:         return "Nic";
    case PWR_OBJ_MEM:         return "Memory";
    case PWR_OBJ_INVALID:     return "Invalid";
    default:                  return "????";
    }
    return NULL;
}

PWR_ObjType XmlConfig::objTypeStrToInt( const std::string name )
{
    if ( 0 == name.compare( "Platform" ) ) {
        return  PWR_OBJ_PLATFORM;
    } else if ( 0 == name.compare( "Cabinet" ) ) {
        return PWR_OBJ_CABINET;
    } else if ( 0 == name.compare( "Chassis" ) ) {
        return PWR_OBJ_CHASSIS;
    } else if ( 0 == name.compare( "Board" ) ) {
        return PWR_OBJ_BOARD;
    } else if ( 0 == name.compare( "Node" ) ) {
        return PWR_OBJ_NODE;
    } else if ( 0 == name.compare( "Socket" ) ) {
        return PWR_OBJ_SOCKET;
    } else if ( 0 == name.compare( "Core" ) ) {
        return PWR_OBJ_CORE;
    } else if ( 0 == name.compare( "Power Plane" ) ) {
        return PWR_OBJ_CORE;
    } else if ( 0 == name.compare( "Memory" ) ) {
        return PWR_OBJ_CORE;
    } else if ( 0 == name.compare( "Nic" ) ) {
        return PWR_OBJ_CORE;
    }
    return PWR_OBJ_INVALID;
}

std::string XmlConfig::attrNameToString( PWR_AttrName name )
{
    switch( name ){
    case PWR_ATTR_PSTATE: return "PSTATE";
    case PWR_ATTR_CSTATE: return "CSTATE";
    case PWR_ATTR_CSTATE_LIMIT: return "CSTATE_LIMIT";
    case PWR_ATTR_SSTATE: return "SSTATE";
    case PWR_ATTR_CURRENT: return "CURRENT";
    case PWR_ATTR_VOLTAGE: return "VOLTAGE";
    case PWR_ATTR_POWER: return "POWER";
    case PWR_ATTR_POWER_LIMIT_MAX: return "MAX_POWER";
    case PWR_ATTR_POWER_LIMIT_MIN: return "MIN_POWER";
    case PWR_ATTR_FREQ: return "FREQ";
    case PWR_ATTR_FREQ_LIMIT_MIN: return "FREQ_MIN";
    case PWR_ATTR_FREQ_LIMIT_MAX: return "FREQ_MAX";
    case PWR_ATTR_ENERGY: return "ENERGY";
    case PWR_ATTR_TEMP: return "TEMP";
    case PWR_ATTR_OS_ID: return "OS_ID";
    case PWR_ATTR_THROTTLED_TIME: return "THROTTLED_TIME";
    case PWR_ATTR_THROTTLED_COUNT: return "THROTTLED_COUNT";
    default: return "";
    }
    return NULL;
}
