
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
#include "./foobar.h"

struct _Grp;
struct _Cntxt;

class _Dev : public Foobar {
  public:

    _Dev( plugin_dev_t* dev, const std::string config ) : m_dev( dev ) {
        m_devInfo = m_dev->open( config.c_str() );
    }

    int attrGetValues( int num, PWR_Value v[], int s[] ){
        DBGX("\n");
        return m_dev->readv( m_devInfo, num, v, s );
    }

    int attrSetValues( int num, PWR_Value v[], int s[] ){ 
        DBGX("\n");
        return m_dev->writev( m_devInfo, num, v, s );
        return PWR_ERR_INVALID;
    }

    int attrGetValue( PWR_AttrType type, void* ptr, size_t len, PWR_Time* ts ){ 
        return m_dev->read( m_devInfo, type, ptr, len, ts ); 
    }

    int attrSetValue( PWR_AttrType type, void* ptr, size_t len ) {
        DBGX("\n");
        return m_dev->write( m_devInfo, type, ptr, len ); 
    }

  private:
    plugin_dev_t* m_dev;
    pwr_dev_t     m_devInfo;
};

struct _Obj : public Foobar{

  public:
    _Obj( _Cntxt* ctx, _Obj* parent, tinyxml2::XMLElement* el );

    std::string& name() { return m_name; }
    PWR_ObjType type();

    _Grp* children();
    _Obj* parent();
    void setParent( _Obj* obj ) { m_parent = obj; }

    int attrGetNumber() { return m_attrVector.size(); }
	
    _Attr* attributeGet( int index ) { 
        return m_attrVector[index]; 
    }

    int attrGetValues( int, PWR_Value [], int [] );
    int attrSetValues( int, PWR_Value [], int [] );
    int attrGetValue( PWR_AttrType, void*, size_t, PWR_Time* );
    int attrSetValue( PWR_AttrType, void*, size_t );

    _Obj* findChild( const std::string name );
    _Dev* findDev( const std::string name, const std::string config );

  private:
    _Cntxt*     m_ctx;
    _Obj*       m_parent; 
    _Grp*       m_children; 
    std::string m_name;
    PWR_ObjType m_type;

    tinyxml2::XMLElement*   m_xmlElement;
    std::vector< _Attr* >   m_attrVector;
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
