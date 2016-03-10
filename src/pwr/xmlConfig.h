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

#ifndef _PWR_XMLCONFIG_H
#define _PWR_XMLCONFIG_H

#include "pwrtypes.h"
#include "config.h"
#include "tinyxml2.h"

namespace PowerAPI {

using namespace tinyxml2;

class XmlConfig : public Config {
  public:
	XmlConfig( std::string file );
	
    std::string findParent( std::string name );
	std::deque< std::string >
                findAttrChildren( std::string, PWR_AttrName );
	std::string findAttrOp( std::string, PWR_AttrName );
	std::string findAttrHz( std::string, PWR_AttrName );

	std::string findAttrType( std::string, PWR_AttrName ) { return "Float"; }
	std::deque< std::string > findChildren( std::string );
	std::deque< Config::ObjDev > findObjDevs( std::string, PWR_AttrName );
	std::deque< Config::Plugin > findPlugins();
	std::deque< Config::SysDev > findSysDevs();
    std::deque< std::string > findObjType( PWR_ObjType );
	virtual bool hasServer( std::string );
	bool hasObject( const std::string name );
	PWR_ObjType objType( const std::string );
	void print( std::ostream& );

  private:
    std::string findObjLocation( std::string );
	XMLNode* findNode( XMLNode*, const std::string name );
	XMLNode* findNodes1stChild( XMLNode*, const std::string name );
	XMLNode* findNodeWithAttr( XMLElement*, const std::string nodeName, 
				const std::string attrName, const std::string attrValue );

	void printTree( std::ostream&, XMLNode* node );
	XMLElement* findObject( const std::string name );
	PWR_ObjType getType( XMLElement* );
	XMLNode* findAttr( XMLElement*, const std::string name );
	XMLNode* findDev( XMLElement*, const std::string name );
	std::string attrNameToString( PWR_AttrName );
	PWR_ObjType objTypeStrToInt( const std::string );
	std::string objTypeToString( PWR_ObjType type );

	XMLDocument* 	m_xml;
	XMLNode* 		m_systemNode;
};

}

#endif
