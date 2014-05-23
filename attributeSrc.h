
#ifndef _PWR_ATTRIBUTESRC_H
#define _PWR_ATTRIBUTESRC_H

#include <string>
#include "./types.h"
#include "mchw.h"


class AttrSrc {
  public:
    virtual int get( void*, size_t ) = 0;
  private:
};

class DevSrc : public AttrSrc {
  public:
    DevSrc( mchw_t* dev, std::string initStr ) : m_dev( dev ) {}
    virtual int get( void*, size_t ) {
        return 0;
    }
    mchw_t* m_dev;
};

class ChildSrc : public AttrSrc {
  public:
    ChildSrc( _Obj* obj ) : m_obj( obj ) {}
    virtual int get( void* ptr, size_t ) {
        *(float*)ptr = 10.12301; 
        return PWR_ERR_SUCCESS;
    }
    
  private:
    _Obj* m_obj;
};

#endif
