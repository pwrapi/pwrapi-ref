
#ifndef _PWR_FOOBAR_H
#define _PWR_FOOBAR_H

#include <vector>
#include "./types.h"

class _Value;
class Foobar {
  public:
    virtual int attrGetValues( const std::vector<PWR_AttrName>& names, void* ptr,
                    std::vector<PWR_Time>& ts, std::vector<int>& status  ) {
        return PWR_RET_FAILURE;
    };
    virtual int attrSetValues( const std::vector<PWR_AttrName>& names, void* ptr, 
                            std::vector<int>& status  ) {
        return PWR_RET_FAILURE;
    };
    virtual int attrGetValue( PWR_AttrName, void*, size_t, PWR_Time* ) {
        return PWR_RET_FAILURE;
    };
    virtual int attrSetValue( PWR_AttrName, void*, size_t ) {
        return PWR_RET_FAILURE;
    };
};


#endif
