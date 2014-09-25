#ifndef _PWR_OBJECT_H
#define _PWR_OBJECT_H

#include <string>
#include <assert.h>
#include "graphNode.h"

class _Cntxt;
class _Grp;
class _Attr;

struct _Obj : public GraphNode {

  public:
    _Obj( _Cntxt* ctx, _Obj* parent ) : 
		m_ctx( ctx ), m_parent( parent ) {}
    virtual ~_Obj() {};

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
    virtual GraphNode* findDev( const std::string name )
	{ assert(0); } 

    virtual int attrIsValid( PWR_AttrName type ) { assert(0); }

  protected:
    _Cntxt*     m_ctx;
    std::string m_name;

  private:
    _Obj*       m_parent; 
};


_Obj* rosebud( _Obj* obj );

#endif
