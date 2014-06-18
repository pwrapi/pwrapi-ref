#ifndef _PWR_OBJECTEL_H 
#define _PWR_OBJECTEL_H

#include "./object.h"

struct _ObjEl : public _Obj
{
  public:
    _ObjEl( _Cntxt* ctx, PWR_Obj parent, tinyxml2::XMLElement* el );
    PWR_ObjType type() { return m_type; }
    _Grp* children();

    int attrGetNumber() { return m_attrVector.size(); }
    _Attr* attributeGet( int index ) { return m_attrVector[index]; }
    int attrIsValid( PWR_AttrName type ) { return 0; }

    _Obj* findChild( const std::string name );
    _Dev* findDev( const std::string name, const std::string config );

    int attrGetValue( PWR_AttrName, void*, size_t, PWR_Time* );
    int attrSetValue( PWR_AttrName, void*, size_t );

    int attrGetValues( const std::vector<PWR_AttrName>& types,
            void* ptr, std::vector<PWR_Time>& ts, std::vector<int>& status );
    int attrSetValues(  const std::vector<PWR_AttrName>& types,
            void* ptr, std::vector<int>& status );
  private:
    PWR_ObjType m_type;
    _Grp*       m_children;

    tinyxml2::XMLElement*   m_xmlElement;
    std::vector< _Attr* >   m_attrVector;
};

inline PWR_Obj createObj( _Cntxt* ctx, PWR_Obj parent,
										tinyxml2::XMLElement* el )
{
	return rosebud( new _ObjEl( ctx, parent, el ) );
}

#endif
