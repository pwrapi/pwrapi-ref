
#ifndef _PWR_ATTRIBUTESRC_H
#define _PWR_ATTRIBUTESRC_H

#include <string>
#include "./types.h"
#include "./object.h"
#include "mchw.h"
#include "./debug.h"


class AttrSrc {
  public:
    AttrSrc( PWR_AttrType type ) : m_type(type) {} 
    virtual int get( void*, size_t ) = 0;
  protected:
    PWR_AttrType    m_type;
    _Obj*           m_obj;
    mchw_t*         m_dev;
  private:
};

class DevSrc : public AttrSrc {
  public:
    DevSrc( PWR_AttrType type, mchw_t* dev, std::string initStr ) : 
        AttrSrc( type )
    {    
        m_dev = dev;
    }

    virtual int get( void*, size_t ) {
//        DBGX("\n");
        return 0;
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
