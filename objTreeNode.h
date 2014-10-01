#ifndef _PWR_OBJECT_H
#define _PWR_OBJECT_H

#include <string>
#include "treeNode.h"
#include "cntxt.h"

namespace PowerAPI {

class Grp;

struct ObjTreeNode : public TreeNode {

  public:
    ObjTreeNode( Cntxt* ctx, std::string name,
					PWR_ObjType type, ObjTreeNode* parent = NULL ) 
	  : TreeNode( ctx ), m_name(name), m_type(type), 
		m_parent(parent), m_children(NULL) {}

    virtual ~ObjTreeNode() {};

    ObjTreeNode* parent() 				{ return m_parent; }
    virtual std::string& name() 		{ return m_name; }
    void setParent( ObjTreeNode* obj )  { m_parent = obj; }
    virtual PWR_ObjType type() 			{ return m_type; }

    virtual Grp* children() {
    	if ( m_children ) return m_children;

    	m_children = m_ctx->findChildren( this );
    	return m_children;
	}


  protected:
    std::string     m_name;
	PWR_ObjType		m_type;
    ObjTreeNode*    m_parent;
	Grp*			m_children;
};

}

#endif
