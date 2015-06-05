/* 
 * Copyright 2014-2015 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#ifndef _PWR_DEVTREENODE_H
#define _PWR_DEVTREENODE_H

#include <string>

#include "debug.h"
#include "treeNode.h"
#include "pwrdev.h"

namespace PowerAPI {

static inline int opAvg( int num, double input[], double* result, PWR_Time* ts)
{
	DBG("\n");
	double total = 0;
	for ( int i = 0; i < num; i++ ) {
		DBG("%f\n",input[i]);
		total += input[i]; 	
	}
	*result = total/num;
	*ts = 0;
	return PWR_RET_SUCCESS;
}

class DevTreeNode : public TreeNode {
  public:

    DevTreeNode( plugin_devops_t* ops, const std::string config )
      : TreeNode(NULL), m_ops( ops ) 
	{
        DBGX("\n");
        m_fd = m_ops->open( ops, config.c_str() );
    }

    int attrGetValues( const std::vector<PWR_AttrName>& names, void* ptr,
                    std::vector<PWR_Time>& ts, std::vector<int>& status ){
        DBGX("\n");
		return m_ops->readv( m_fd, names.size(), &names[0], ptr,
                            &ts[0], &status[0] );
    }

    int attrSetValues( const std::vector<PWR_AttrName>& names, void* ptr,
                    std::vector<int>& status ){
        DBGX("\n");
        return m_ops->writev( m_fd, names.size(), &names[0], ptr,
                                                            &status[0] );
    }

    int attrGetValue( PWR_AttrName name, void* ptr, size_t len, PWR_Time* ts ){
        DBGX("\n");
        return m_ops->read( m_fd, name, ptr, len, ts );
    }

    int attrSetValue( PWR_AttrName name, void* ptr, size_t len ) {
        DBGX("\n");
        return m_ops->write( m_fd, name, ptr, len );
    }

	int attrStartStat( PWR_AttrName name, PWR_AttrStat stat) {
        DBGX("\n");
		if ( m_ops->stat_start ) {
			return m_ops->stat_start( m_fd, name );
		} else {
			return PWR_RET_FAILURE;
		}
	} 

	int attrStopStat( PWR_AttrName name, PWR_AttrStat stat) {
        DBGX("\n");
		if ( m_ops->stat_stop ) {
			return m_ops->stat_stop( m_fd, name );
		} else {
			return PWR_RET_FAILURE;
		}
	} 

	int attrClearStat( PWR_AttrName name, PWR_AttrStat stat) {
        DBGX("\n");
		if ( m_ops->stat_clear ) {
			return m_ops->stat_clear( m_fd, name );
		} else {
			return PWR_RET_FAILURE;
		}
	} 

	int attrGetStat( PWR_AttrName name, PWR_AttrStat stat, double* result, PWR_StatTimes* ts) {
        DBGX("\n");
		if ( m_ops->stat_get ) {
			return m_ops->stat_get( m_fd, name, opAvg, result, ts );
		} else {
			return PWR_RET_FAILURE;
		}
	} 

    ~DevTreeNode() {
        m_ops->close( m_fd );
    }

  private:
    plugin_devops_t*   m_ops;
    pwr_fd_t        m_fd;
};

}

#endif
