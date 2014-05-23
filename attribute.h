
#ifndef _PWR_ATTRIBUTE_H
#define _PWR_ATTRIBUTE_H

#include <deque>
#include <string>
#include "./types.h"
#include "./attributeSrc.h"
#include "tinyxml2.h"

class _Attr {
  public:
    _Attr( _Obj* obj, tinyxml2::XMLElement* el  );
    virtual ~_Attr() {
        if ( m_srcList ) delete m_srcList;
    }

    PWR_AttrType type() { return m_type; }  
    PWR_AttrDataType dataType() { return m_dataType; }
    int getValue( void*, size_t len );
    int setValue( void*, size_t len );

    typedef std::deque< AttrSrc* > srcList_t;
  private:

    srcList_t* initSrcList( tinyxml2::XMLElement* el );

    std::string             m_name;
    _Obj*                   m_obj;
    tinyxml2::XMLElement*   m_xml;
    PWR_AttrType            m_type;
    PWR_AttrDataType        m_dataType;
    size_t                  m_len;
    
    srcList_t*              m_srcList;
};

#endif
