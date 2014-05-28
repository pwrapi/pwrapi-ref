
#ifndef _PWR_ATTRIBUTESRC_H
#define _PWR_ATTRIBUTESRC_H

#include <string>
#include "./types.h"
#include "./object.h"
#include "./dev.h"
#include "./debug.h"


class AttrSrc {
  public:
    AttrSrc( PWR_AttrType type ) : m_type(type) {} 
    virtual int get( void*, size_t ) = 0;
  protected:
    PWR_AttrType    m_type;
    _Obj*           m_obj;
    plugin_dev_t*   m_dev;
    pwr_dev_t*      m_devInfo;
  private:
};

class DevSrc : public AttrSrc {
  public:
    DevSrc( PWR_AttrType type, plugin_dev_t* dev, std::string initStr ) : 
        AttrSrc( type )
    {    
        m_dev = dev;
#if 0
        assert(0);
        m_devInfo = dev->open( initStr.c_str());
        assert( m_devInfo );
#endif
        m_devInfo = NULL;
    }

    virtual int get( void* ptr, size_t len ) {
//        DBGX("\n");
        PWR_AttrType type = PWR_ATTR_POWER;
        float value; 
        assert( m_devInfo );
        unsigned long long ts;
        return m_dev->read( m_devInfo, 1, &type, &value, &ts );
    }
};

class ChildSrc : public AttrSrc {
  public:
    ChildSrc( PWR_AttrType type, _Obj* obj ) :
        AttrSrc( type ) 
    {
        m_obj = obj;
    }

    virtual int get( void* ptr, size_t len ) {
//        DBGX("\n");
        return m_obj->attrGetValue( m_type, ptr, len );
    }
};

#endif
