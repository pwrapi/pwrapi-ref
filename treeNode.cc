#include <string>
#include <map>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>

#include "util.h"
#include "debug.h"
#include "treeNode.h"
#include "config.h"
#include "cntxt.h"

using namespace PowerAPI;

TreeNode::TreeNode( Cntxt* ctx ) 
	: m_ctx(ctx)
{
    m_attrMap.resize( PWR_ATTR_INVALID, AttrEntry(this, m_ctx) );
}

void TreeNode::AttrEntry::init( PWR_AttrName attr )
{
    DBGX("%s\n", attrNameToString(attr) );
	m_attrName = attr;
	std::string op;
	m_ctx->initAttr( m_node, *this );
}

int TreeNode::attrIsValid( PWR_AttrName attr )
{
	assert(0);
}

int TreeNode::attrGetValue( PWR_AttrName attr, void* valuePtr,
											size_t len, PWR_Time* ts )
{
    DBGX("%s\n", attrNameToString(attr));

	AttrEntry& entry = m_attrMap[attr];
	std::vector<TreeNode*>&  nodes = entry.nodes(); 

	if ( ! entry.inited() ) {
		entry.init( attr );
	}
	
	if ( nodes.empty() ) {
		return PWR_RET_INVALID;
	}

    unsigned char * buf = (unsigned char* )malloc( len * nodes.size() );

    for ( unsigned int i = 0; i < nodes.size();  i++ ) {

		DBGX("\n");
        PWR_Time _ts;
        nodes[i]->attrGetValue( attr, buf + len * i, len, &_ts );
        *ts = _ts;
    }

    assert( entry.op );
    entry.op( nodes.size(), valuePtr, buf );

    return PWR_RET_SUCCESS;
}

int TreeNode::attrSetValue( PWR_AttrName attr, void* valuePtr, size_t len )
{
    DBGX("%s\n", attrNameToString(attr));

	AttrEntry& entry = m_attrMap[attr];
	std::vector<TreeNode*>&  nodes = entry.nodes(); 

	if ( ! entry.inited() ) {
		entry.init( attr );
	}

	if ( nodes.empty() ) {
		return PWR_RET_INVALID;
	}

    for ( unsigned int i = 0; i <  nodes.size(); i++ ) {

        nodes[i]->attrSetValue( attr, valuePtr, len );
    }

    return PWR_RET_SUCCESS;
}

struct Tmp {
    std::vector<PWR_AttrName> attrs;
    std::vector<uint64_t>     data;
    std::vector<int>          status;

    std::vector<PWR_Time>     ts;
    std::vector<int>          attrPos;
};

int TreeNode::attrGetValues( const std::vector<PWR_AttrName>& attrs,
		void* valuePtr, std::vector<PWR_Time>& ts, std::vector<int>& status )
{
    int retval = PWR_RET_SUCCESS;

    std::map< TreeNode*, Tmp > tmpMap;

    std::map< PWR_AttrName, std::vector<void*> > opInput;

    DBGX("enter\n");

    for ( unsigned int i=0; i < attrs.size(); i++ ) {

		AttrEntry& entry = m_attrMap[ attrs[i] ];
		std::vector<TreeNode*>&  nodes = entry.nodes(); 

		if ( ! entry.inited() ) {
			entry.init( attrs[i] );
		}

		opInput[ attrs[i] ].resize( nodes.size() );

        for ( unsigned int j = 0; j < nodes.size(); j++ ) {

            Tmp& tmp = tmpMap[ nodes[j] ];
            tmp.attrs.push_back( attrs[i] );
            tmp.attrPos.push_back( j );
        }
    }

    std::map<TreeNode*, Tmp >::iterator iter = tmpMap.begin();
    for ( ; iter != tmpMap.end(); ++iter ) {
        Tmp& tmp = (*iter).second;

        tmp.data.resize( tmp.attrs.size() );
        tmp.ts.resize( tmp.attrs.size() );
        tmp.status.resize( tmp.attrs.size() );

        (*iter).first->attrGetValues( tmp.attrs, &tmp.data[0],
												tmp.ts, tmp.status );

        for ( unsigned int i = 0; i < tmp.attrs.size(); i++ ) {

            if ( PWR_RET_SUCCESS != tmp.status[i] ) {
                retval = PWR_RET_FAILURE;
            }

            opInput[ tmp.attrs[i] ][ tmp.attrPos[i] ] = &tmp.data[i];
        }
    }

	std::map< PWR_AttrName, std::vector<void*> >::
										iterator iter2 = opInput.begin();

    for ( int pos = 0; iter2 != opInput.end(); ++iter2, pos++ ) {

		int attr = iter2->first;
		AttrEntry& entry = m_attrMap[ attr ];
    	assert( entry.op2 );

        DBGX("call op2 \n");

    	entry.op2( (void*) &((uint64_t*)valuePtr)[pos], iter2->second  );

        // Should the calculation of the timestamp be moved to the "op"? 
        struct timeval tv;
        gettimeofday(&tv,NULL);

        ts[pos] = tv.tv_sec * 1000000000;
        ts[pos] += tv.tv_usec * 1000;
    }

    DBGX("leave\n");

    return retval;
}

int TreeNode::attrSetValues( const std::vector<PWR_AttrName>& attrs,
									void* valuePtr, std::vector<int>& status  )
{
    int retval = PWR_RET_SUCCESS;

    std::map< TreeNode*, Tmp > tmpMap;


    for ( unsigned int i=0; i < attrs.size(); i++ ) {

		AttrEntry& entry = m_attrMap[ attrs[i] ];
		std::vector<TreeNode*>&  nodes = entry.nodes(); 

		if ( ! entry.inited() ) {
			entry.init( attrs[i] );
		}

        for ( unsigned int j = 0; j < nodes.size(); j++ ) {

        	DBGX("TreeNode %p for attr=%s \n", nodes[j],
											attrNameToString(attrs[i]) );

            Tmp& tmp = tmpMap[ nodes[j] ];
            tmp.attrs.push_back( attrs[i] );
            tmp.data.push_back( ( ( uint64_t* ) valuePtr)[i] );
        }
    }

    std::map<TreeNode*, Tmp >::iterator iter = tmpMap.begin();
    for ( ; iter != tmpMap.end(); ++iter ) {

        Tmp& tmp = (*iter).second;
        tmp.status.resize( tmp.attrs.size() );

        (*iter).first->attrSetValues( tmp.attrs, &tmp.data[0], tmp.status );

        for ( unsigned int i = 0; i < tmp.attrs.size(); i++ ) {
            if ( PWR_RET_SUCCESS != tmp.status[i] ) {
                retval = PWR_RET_FAILURE;
            }
        }
    }

    return retval;
}
