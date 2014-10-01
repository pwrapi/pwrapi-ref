
#ifndef _PWR_XMLCONFIG_H
#define _PWR_XMLCONFIG_H

#include "types.h"
#include "config.h"
#include "tinyxml2.h"

namespace PowerAPI {

using namespace tinyxml2;

class XmlConfig : public Config {
  public:
	XmlConfig( std::string file );
	
	std::deque< std::string >
                findAttrChildren( std::string, PWR_AttrName );
	std::string findAttrOp( std::string, PWR_AttrName );
	std::deque< std::string > findChildren( std::string );
	std::deque< Config::ObjDev > findObjDevs( std::string, PWR_AttrName );
	std::deque< Config::Plugin > findPlugins();
	std::deque< Config::SysDev > findSysDevs();
    std::deque< std::string > findObjType( PWR_ObjType );
	bool findObject( const std::string name );
	PWR_ObjType objType( const std::string );
	void print( std::ostream& );

  private:
	XMLNode* findNode( XMLNode*, const std::string name );

	void printTree( std::ostream&, XMLNode* node );
	XMLElement* findObject_p( const std::string name );
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
