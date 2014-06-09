
#ifndef _PWR_FOOBAR_H
#define _PWR_FOOBAR_H

#include "./types.h"

class _Value;
class Foobar {
  public:
    virtual int attrGetValues( const std::vector<PWR_AttrType>& types, void* ptr,
                    std::vector<unsigned long long>& ts, std::vector<int>& status  ) {
        return PWR_ERR_FAILURE;
    };
    virtual int attrSetValues( const std::vector<PWR_AttrType>& types, void* ptr, 
                            std::vector<int>& status  ) {
        return PWR_ERR_FAILURE;
    };
    virtual int attrGetValue( PWR_AttrType, void*, size_t, unsigned long long* ) {
        return PWR_ERR_FAILURE;
    };
    virtual int attrSetValue( PWR_AttrType, void*, size_t ) {
        return PWR_ERR_FAILURE;
    };
};


#endif
