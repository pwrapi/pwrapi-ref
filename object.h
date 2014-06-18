#ifndef _PWR_OBJECT_H
#define _PWR_OBJECT_H

#include <string>
#include <assert.h>

#include "./attribute.h"
#include "./debug.h"
#include "./dev.h"
#include "./foobar.h"

class _Cntxt;
class _Grp;

class _Dev : public Foobar {
  public:

    _Dev( plugin_dev_t* dev, const std::string config ) : m_dev( dev ) {
        m_devInfo = m_dev->open( config.c_str() );
    }

    int attrGetValues( const std::vector<PWR_AttrName>& names, void* ptr,
                    std::vector<PWR_Time>& ts, std::vector<int>& status ){
        DBGX("\n");
        return m_dev->readv( m_devInfo, names.size(), &names[0], ptr,  
                            &ts[0], &status[0] );
    }

    int attrSetValues( const std::vector<PWR_AttrName>& names, void* ptr,
                    std::vector<int>& status ){ 
        DBGX("\n");
        return m_dev->writev( m_devInfo, names.size(), &names[0], ptr, &status[0] );
    }

    int attrGetValue( PWR_AttrName name, void* ptr, size_t len, PWR_Time* ts ){ 
        return m_dev->read( m_devInfo, name, ptr, len, ts ); 
    }

    int attrSetValue( PWR_AttrName name, void* ptr, size_t len ) {
        DBGX("\n");
        return m_dev->write( m_devInfo, name, ptr, len ); 
    }
	~_Dev() {
		m_dev->close( m_devInfo );
	}

  private:
    plugin_dev_t* m_dev;
    pwr_dev_t     m_devInfo;
};

struct _Obj : public Foobar{

  public:
    _Obj( _Cntxt* ctx, _Obj* parent ) : 
		m_ctx( ctx ), m_parent( parent ) {}

    _Obj* parent() { return m_parent; }
    void setParent( _Obj* obj ) { m_parent = obj; }

    virtual _Grp* children() { assert(0); }
    virtual std::string& name() { return m_name; }
    virtual PWR_ObjType type() { assert(0); }
    virtual int attrGetNumber() { assert(0); }
    virtual _Attr* attributeGet( int index ) { assert(0); }

    virtual int attrGetValues( const std::vector<PWR_AttrName>& types,
			void* ptr, std::vector<PWR_Time>& ts, std::vector<int>& status )
	{ assert(0); } 
    virtual int attrSetValues(  const std::vector<PWR_AttrName>& types,
			void* ptr, std::vector<int>& status )
	{ assert(0); } 

    virtual int attrGetValue( PWR_AttrName, void*, size_t, PWR_Time* ) 
	{ assert(0); } 
    virtual int attrSetValue( PWR_AttrName, void*, size_t )
	{ assert(0); } 

    virtual _Obj* findChild( const std::string name ) { assert(0); }
    virtual _Dev* findDev( const std::string name, const std::string config )
	{ assert(0); } 

    virtual int attrIsValid( PWR_AttrName type ) { assert(0); }

  protected:
    _Cntxt*     m_ctx;
    std::string m_name;

  private:
    _Obj*       m_parent; 
};


PWR_Obj rosebud( _Obj* obj );

#endif
