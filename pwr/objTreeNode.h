/* 
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#ifndef _PWR_OBJTREENODE_H
#define _PWR_OBJTREENODE_H

#include <string>
#include "treeNode.h"
#include "cntxt.h"
#include "debug.h"

namespace PowerAPI {

class Grp;

class ObjTreeNode : public TreeNode {

  public:
    ObjTreeNode( Cntxt* ctx, std::string name,
					PWR_ObjType type, ObjTreeNode* parent = NULL ) 
	  : TreeNode( ctx ), m_name(name), m_type(type), 
		m_parent(parent), m_children(NULL) 
    {
        DBGX("name=%s\n",name.c_str());
    }

    virtual ~ObjTreeNode() {}

    ObjTreeNode* parent() 				{ return m_parent; }
    virtual std::string& name() 		{ return m_name; }
    void setParent( ObjTreeNode* obj )  { m_parent = obj; }
    virtual PWR_ObjType type() 			{ return m_type; }

	virtual int attrStartStat( PWR_AttrName attr, PWR_AttrStat stat) {
		DBGX("%s\n",objTypeToString(m_type));
	    assert(!m_children);
		return TreeNode::attrStartStat(attr,stat); 
	}

	virtual int attrStopStat( PWR_AttrName attr, PWR_AttrStat stat) {
		DBGX("%s\n",objTypeToString(m_type));
	    assert(!m_children);
		return TreeNode::attrStopStat(attr,stat); 
	}

	virtual int attrClearStat( PWR_AttrName attr, PWR_AttrStat stat) {
		DBGX("%s\n",objTypeToString(m_type));
	    assert(!m_children);
		return TreeNode::attrClearStat(attr,stat); 
	}

    virtual int attrGetStat( PWR_AttrName attr, PWR_AttrStat stat,
                double* value, PWR_StatTimes* statTimes )
	{
		DBGX("%s\n",objTypeToString(m_type));
    	assert(!m_children);
    	return TreeNode::attrGetStat( attr, stat, value, statTimes );
	}

    virtual int attrGetStats( PWR_AttrName attr, PWR_AttrStat stat,
                double* value, PWR_StatTimes* statTimes )
	{
	    DBGX("%s\n",objTypeToString(m_type));
    	assert(!m_children);
    	return TreeNode::attrGetStats( attr, stat, value, statTimes );
	}

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
