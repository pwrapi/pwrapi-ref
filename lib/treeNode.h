#ifndef _PWR_TREENODE_H
#define _PWR_TREENODE_H

#include <vector>
#include <assert.h>
#include "types.h"

namespace PowerAPI {

class Cntxt;

class TreeNode {
  public:
	TreeNode( Cntxt* ctx );
	virtual ~TreeNode() {}

    virtual int attrGetValues( const std::vector<PWR_AttrName>& names,
		void* ptr, std::vector<PWR_Time>& ts, std::vector<int>& status  );
    virtual int attrSetValues( const std::vector<PWR_AttrName>& names,
		void* ptr, std::vector<int>& status  );
    virtual int attrGetValue( PWR_AttrName, void*, size_t, PWR_Time* );
    virtual int attrSetValue( PWR_AttrName, void*, size_t );
	virtual int attrIsValid( PWR_AttrName );

	virtual int attrStartStat( PWR_AttrName, PWR_AttrStat ); 
	virtual int attrStopStat( PWR_AttrName, PWR_AttrStat);
	virtual int attrClearStat( PWR_AttrName, PWR_AttrStat);
	virtual int attrGetStat( PWR_AttrName, PWR_AttrStat, 
				double* value, PWR_StatTimes* statTimes );
	virtual int attrGetStats( PWR_AttrName, PWR_AttrStat, 
				double* value, PWR_StatTimes* statTimes );
	
	Cntxt* getCtx() { return m_ctx; }

	class AttrEntry  {
      public:
	    AttrEntry( TreeNode* node, Cntxt* ctx ) 
			: op(NULL), op2(NULL), m_node(node),
				m_ctx(ctx), m_attrName(PWR_ATTR_INVALID) {}

		bool inited() { return m_attrName != PWR_ATTR_INVALID; }

		void init( PWR_AttrName );
		void (*op)( int num, void* out, void* in );
		void (*op2)( void*, const std::vector< void* >& );

		PWR_AttrName name() { return m_attrName; }
		void addSrc( TreeNode* node ) { m_nodes.push_back( node ); }
		std::vector< TreeNode* >& nodes() { return m_nodes; }	

		void setOp( void (*_op)( int num, void* out, void* in )  ) {
			op = _op;
		}
		void setOp( void (*_op)( void*, const std::vector< void* >& ) ) {
			op2 = _op;
		}
		
      private:
		std::vector< TreeNode* > m_nodes;	

		TreeNode*  		m_node;
		Cntxt*			m_ctx;
		PWR_AttrName 	m_attrName;
	};

  protected:

	Cntxt*			m_ctx;

	std::vector< AttrEntry > m_attrMap;
};

}

#endif
