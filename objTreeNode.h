#ifndef _PWR_OBJECT_H
#define _PWR_OBJECT_H

#include <string>
#include <assert.h>
#include "treeNode.h"

class _Grp;
class _Attr;

struct ObjTreeNode : public TreeNode {

  public:
    ObjTreeNode( _Cntxt* ctx, TreeNode* parent ) : 
		TreeNode( ctx, parent ) {}
    virtual ~ObjTreeNode() {};

    void setParent( TreeNode* obj ) { m_parent = obj; }

    virtual _Grp* children() { assert(0); }
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

    virtual TreeNode* findChild( const std::string name ) { assert(0); }
    virtual TreeNode* findDev( const std::string name )
	{ assert(0); } 

    virtual int attrIsValid( PWR_AttrName type ) { assert(0); }
};


TreeNode* rosebud( TreeNode* obj );

#endif
