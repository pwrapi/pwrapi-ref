
#ifndef _PWR_ATTRIBUTE_H
#define _PWR_ATTRIBUTE_H

#include <deque>
#include <vector>
#include <string>
#include "types.h"
#include "tinyxml2.h"

class AttrSrc;
class TreeNode;

class _Attr  {

  public:
    _Attr( TreeNode* obj, tinyxml2::XMLElement* el  );

    std::vector<TreeNode*>& foobar();
    PWR_AttrName name() { return m_name; }  
    PWR_AttrDataType dataType() { return m_dataType; }
    int getValue( void*, size_t len, PWR_Time* ts );
    int setValue( void*, size_t len );
    void op( void*, const std::vector<void*>& );

  private:

    void initSrcList( tinyxml2::XMLElement* el );

    TreeNode*              m_obj;
    tinyxml2::XMLElement*   m_xml;
    PWR_AttrName            m_name;
    PWR_AttrDataType        m_dataType;
    size_t                  m_len;
    
    void              (*m_op)( int num, void* out, void* in ); 
    void              (*m_op2)( void*, const std::vector< void* >& ); 

    std::vector<TreeNode*>    m_foobar;
};

#endif
