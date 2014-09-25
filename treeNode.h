#ifndef _PWR_TREENODE_H
#define _PWR_TREENODE_H

#include <vector>
#include "types.h"

class TreeNode {
  public:
	TreeNode( TreeNode* parent = NULL ) : m_parent(parent) {}

	TreeNode* parent() { return m_parent; }
	virtual std::string& name() { return m_name; }

    virtual int attrGetValues( const std::vector<PWR_AttrName>& names,
		void* ptr, std::vector<PWR_Time>& ts, std::vector<int>& status  ) {
		assert(0);
    };
    virtual int attrSetValues( const std::vector<PWR_AttrName>& names,
		void* ptr, std::vector<int>& status  ) {
		assert(0);
    };
    virtual int attrGetValue( PWR_AttrName, void*, size_t, PWR_Time* ) {
		assert(0);
    };
    virtual int attrSetValue( PWR_AttrName, void*, size_t ) {
		assert(0);
	};
  protected:
	std::string 	m_name;
    TreeNode*       m_parent;

};

#endif
