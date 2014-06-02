
#ifndef _PWR_FOOBAR_H
#define _PWR_FOOBAR_H

#include "./types.h"

class _Value;
class Foobar {
  public:
    virtual int attrGetValues( int, PWR_Value [], int [] ) {
        return PWR_ERR_FAILURE;
    };
    virtual int attrSetValues( int, PWR_Value [], int [] ) {
        return PWR_ERR_FAILURE;
    };

    virtual int attrGetValue( PWR_AttrType, void*, size_t, PWR_Time* ) {
        return PWR_ERR_FAILURE;
    };
    virtual int attrSetValue( PWR_AttrType, void*, size_t ) {
        return PWR_ERR_FAILURE;
    };
};


#endif
