
#ifndef PWR_OBJECT_H
#define PWR_OBJECT_H

#include <assert.h>
#include <string>
#include <vector>
#include <map>

#include "tinyxml2.h"

#include "./types.h"
#include "./util.h"
#include "./attribute.h"

struct _Obj;
struct _Cntxt;

struct _Grp {
  public:
    _Grp( _Cntxt* ctx ) : m_ctx(ctx) { reset(); }

    long size() { return m_list.size(); }
    bool empty() { return m_list.empty(); }

    _Obj* getObj( int i ) { return m_list[i]; }

    void reset() { pos = m_list.begin(); }

    _Obj* next() {
        if ( pos == m_list.end() ) {
            return PWR_NULL;
        } else {
            _Obj* tmp = *pos;
            ++pos;
            return tmp;
        }
    }

    int add( _Obj* obj ) {
        m_list.push_back( obj );
        return PWR_ERR_SUCCESS; 
    }

    int remove( _Obj* obj ) {
        std::vector<_Obj*>::iterator iter = m_list.begin();
        for ( ; iter != m_list.end(); ++iter ) {
            if ( *iter == obj ) {
                m_list.erase( iter );
                break;
            }
        }
        return PWR_ERR_SUCCESS;
    }
    _Cntxt* getCtx() { return m_ctx; }

  private:
    std::vector<_Obj*> m_list;
    std::vector<_Obj*>::iterator pos;
    _Cntxt*   m_ctx;
};

struct _Obj {

  public:
    _Obj(_Cntxt* ctx, tinyxml2::XMLElement* el );

    std::string& name() { return m_name; }
    PWR_ObjType type();

    _Grp* children();
    _Obj* parent();
    void setParent( _Obj* obj ) { m_parent = obj; }

    int attrGetNumber() { return m_attrVector.size(); }
	
    _Attr* findAttrType( PWR_AttrType type );

    _Attr* attributeGet( int index ) { 
        return m_attrVector[index]; 
    }

    void attrAdd( _Attr* attr ) {
        m_attrVector.push_back( attr );
        m_attrMap[ attr->type() ] = attr; 
    }
    _Obj* findChild( std::string name ) {
        return NULL;
    }

  private:
    _Cntxt*     m_ctx;
    tinyxml2::XMLElement* m_xmlElement;
    std::string m_name;
    PWR_ObjType m_type;
    _Obj*       m_parent; 
    _Grp*       m_children; 
    
    std::vector< _Attr* > m_attrVector;
    std::map< int, _Attr* > m_attrMap;
};

#include "./cntxt.h"

#endif
