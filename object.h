
#ifndef PWR_OBJECT_H
#define PWR_OBJECT_H

#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>

#include "tinyxml2.h"

#include "./types.h"
#include "./util.h"
#include "./attribute.h"
#include "./debug.h"
#include "./dev.h"

struct _Grp;
struct _Cntxt;


struct _Obj {

  public:
    _Obj( _Cntxt* ctx, _Obj* parent, tinyxml2::XMLElement* el );

    std::string& name() { return m_name; }
    PWR_ObjType type();

    _Grp* children();
    _Obj* parent();
    void setParent( _Obj* obj ) { m_parent = obj; }

    int attrGetNumber() { return m_attrVector.size(); }
	
    _Attr* attrFindType( PWR_AttrType type );

    _Attr* attributeGet( int index ) { 
        return m_attrVector[index]; 
    }

    void attrAdd( _Attr* attr ) {
        m_attrVector.push_back( attr );
        m_attrMap[ attr->type() ] = attr; 
    }

    int attrGetValue( PWR_AttrType type, void* ptr, size_t len ) {
        DBGX("%s \n",name().c_str());
        return attrFindType(type)->getValue( ptr, len ); 
    }

    int attrSetValue( PWR_AttrType type, void* ptr, size_t len ) {
        DBGX("%s \n",name().c_str());
        return attrFindType( type)->setValue( ptr, len ); 
    }

    _Obj* findChild( const std::string name );
    plugin_dev_t* findDev( const std::string name );

  private:
    _Cntxt*     m_ctx;
    _Obj*       m_parent; 
    _Grp*       m_children; 
    tinyxml2::XMLElement* m_xmlElement;
    std::string m_name;
    PWR_ObjType m_type;
    
    std::vector< _Attr* > m_attrVector;
    std::map< int, _Attr* > m_attrMap;
};

struct _Grp {
  public:
    _Grp( _Cntxt* ctx ) : m_ctx(ctx) {  }

    long size() { return m_list.size(); }
    bool empty() { return m_list.empty(); }

    _Obj* getObj( int i ) { return m_list[i]; }

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

    _Obj* find( std::string name ) {
        DBGX("%s\n", name.c_str());
        
        for ( unsigned int i = 0; i < m_list.size(); i++ ) {
            _Obj* obj = m_list[i];
            if ( obj->parent() ) {
                std::string tmp = m_list[i]->parent()->name() + "." + name;
                DBGX("%s %s\n",tmp.c_str(), obj->name().c_str() );
                if ( 0 == tmp.compare( obj->name() ) ) { 
                    return obj;
                }
            }
        }
        return NULL;
    }

    _Cntxt* getCtx() { return m_ctx; }

  private:
    _Cntxt*   m_ctx;
    std::vector<_Obj*> m_list;
};

#include "./cntxt.h"

#endif
