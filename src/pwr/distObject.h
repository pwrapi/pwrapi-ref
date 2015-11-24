
#ifndef _DIST_OBJECT_H
#define _DIST_OBJECT_H

#include "object.h"

namespace PowerAPI {

class Request;
class Status;

class DistObject : public Object {

  public:
	DistObject( std::string name, PWR_ObjType type, Cntxt* ctx ) : 
		Object( name, type, ctx ) {} 

    virtual int attrGetValue( PWR_AttrName attr, void* buf, 
								PWR_Time* ts );
    virtual int attrSetValue( PWR_AttrName attr, void* buf );
	virtual int attrGetValues( int count, PWR_AttrName names[],
								void* buf, PWR_Time ts[], Status* );
    virtual int attrSetValues( int count, PWR_AttrName names[],
								void* buf, Status*  );

	virtual int attrGetValue( PWR_AttrName , void* buf,
								PWR_Time*, Request* );
	virtual int attrSetValue( PWR_AttrName , void* buf, Request* );

	virtual int attrGetValues( int count, PWR_AttrName names[],
							void* buf, PWR_Time ts[], Status*, Request* );
	virtual int attrSetValues( int count, PWR_AttrName names[],
							void* buf, Status*, Request* );

    virtual int attrStartLog( PWR_AttrName );
    virtual int attrStopLog( PWR_AttrName );
    virtual int attrGetSamples( PWR_AttrName name, PWR_Time*, double period,
                                       unsigned int* count, void* buf );
    virtual int attrStartLog( PWR_AttrName, Request* );
    virtual int attrStopLog( PWR_AttrName, Request* );
    virtual int attrGetSamples( PWR_AttrName name, PWR_Time*, double period,
                       unsigned int* count, void* buf, Request* );
};

};

#endif
