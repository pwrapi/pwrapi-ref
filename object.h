
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
#include "./status.h"

struct _Grp;
struct _Cntxt;

class _Dev : public Foobar {
  public:

    _Dev( plugin_dev_t* dev, const std::string config ) : m_dev( dev ) {
        m_devInfo = m_dev->open( config.c_str() );
    }

    int attrGetValues( const std::vector<PWR_AttrType>& types, void* ptr,
                    std::vector<PWR_Time>& ts, std::vector<int>& status ){
        DBGX("\n");
        return m_dev->readv( m_devInfo, types.size(), &types[0], ptr,  
                            &ts[0], &status[0] );
    }

    int attrSetValues( const std::vector<PWR_AttrType>& types, void* ptr,
                    std::vector<int>& status ){ 
        DBGX("\n");
        return m_dev->writev( m_devInfo, types.size(), &types[0], ptr, &status[0] );
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

    int attrGetValues( const std::vector<PWR_AttrType>& types, void* ptr,
                    std::vector<PWR_Time>& ts, std::vector<int>& status );
    int attrSetValues(  const std::vector<PWR_AttrType>& types, void* ptr,
                    std::vector<int>& status );

    int attrGetValue( PWR_AttrType, void*, size_t, PWR_Time* );
    int attrSetValue( PWR_AttrType, void*, size_t );

    _Obj* findChild( const std::string name );
    _Dev* findDev( const std::string name, const std::string config );

    int attrIsValid( PWR_AttrType type ) {
        return 0;
    }

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
    _Grp( _Cntxt* ctx, const std::string name ="" ) : m_ctx(ctx), m_name(name) {  }

    long size() { return m_list.size(); }
    bool empty() { return m_list.empty(); }

    _Obj* getObj( int i ) { return m_list[i]; }

    int add( _Obj* obj ) {
        m_list.push_back( obj );
        return PWR_ERR_SUCCESS; 
    }
    const std::string& name() {
        return m_name;
    }

    
    int attrSetValue( PWR_AttrType type, void* ptr, size_t len, PWR_Status status ) {
        for ( unsigned int i = 0; i < m_list.size(); i++ ) {
            int ret = m_list[i]->attrSetValue( type, ptr, len );
            if ( PWR_ERR_SUCCESS != ret ) {
                status->add( m_list[i], type, ret );
            }
        }
        if ( !status->empty() ) {
            return PWR_ERR_FAILURE;
        } else {
            return PWR_ERR_SUCCESS;
        }
    }

    int attrSetValues( int num, PWR_AttrType attr[], void* buf,
                                                        PWR_Status status )
    {
        for ( unsigned int i = 0; i < m_list.size(); i++ ) {

            std::vector<PWR_AttrType> attrsV(num);
            std::vector<int>          statusV(num);
        
            if ( PWR_ERR_SUCCESS != m_list[i]->attrSetValues( attrsV, buf, statusV ) ) {

                for ( int j = 0; j < num; j++ ) {
                    if ( PWR_ERR_SUCCESS != statusV[j] ) {
                        status->add( m_list[i], attrsV[j], statusV[j] );
                    }
                }
            }
        }
        if ( !status->empty() ) {
            return PWR_ERR_FAILURE;
        } else {
            return PWR_ERR_SUCCESS;
        }
    }

    int attrGetValues( int num, PWR_AttrType attr[], void* buf,
                                                        PWR_Time ts[], PWR_Status status)
    {
        uint64_t* ptr = (uint64_t*) buf;
        for ( unsigned int i = 0; i < m_list.size(); i++ ) {

            std::vector<PWR_AttrType> attrsV(num);
            std::vector<PWR_Time> tsV(num);
            std::vector<int>          statusV(num);

            for ( int j = 0; j < num; j++ ) {
                attrsV[j] =  attr[ i * num + j ];
            }

            if ( PWR_ERR_SUCCESS != m_list[i]->attrGetValues( attrsV, 
                        ptr + i * num, tsV, statusV ) ) {

                for ( int j = 0; j < num; j++ ) {
                    if ( PWR_ERR_SUCCESS != statusV[j] ) {
                        status->add( m_list[i], attrsV[j], statusV[j] );
                    }
                }
            }
            for ( int j = 0; j < num; j++ ) {
                ts[ i * num + j ] = tsV[j];
            }
        }

        if ( !status->empty() ) {
            return PWR_ERR_FAILURE;
        } else {
            return PWR_ERR_SUCCESS;
        }
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
    std::string m_name;
};

#include "./cntxt.h"

#endif
