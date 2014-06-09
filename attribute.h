
#ifndef _PWR_ATTRIBUTE_H
#define _PWR_ATTRIBUTE_H

#include <deque>
#include <vector>
#include <string>
#include "./types.h"
#include "tinyxml2.h"

class AttrSrc;
class Foobar;

class _Attr  {

  public:
    _Attr( _Obj* obj, tinyxml2::XMLElement* el  );

    std::vector<Foobar*>& foobar();
    PWR_AttrType type() { return m_type; }  
    PWR_AttrDataType dataType() { return m_dataType; }
    int getValue( void*, size_t len, PWR_Time* ts );
    int setValue( void*, size_t len );
    void op( void*, const std::vector<void*>& );

  private:

    void initSrcList( tinyxml2::XMLElement* el );

    std::string             m_name;
    _Obj*                   m_obj;
    tinyxml2::XMLElement*   m_xml;
    PWR_AttrType            m_type;
    PWR_AttrDataType        m_dataType;
    size_t                  m_len;
    
    void              (*m_op)( int num, void* out, void* in ); 
    void              (*m_op2)( void*, const std::vector< void* >& ); 

    std::vector<Foobar*>    m_foobar;
};

#endif
