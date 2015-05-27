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

#ifndef _PWR_RPCTREENODE_H
#define _PWR_RPCTREENODE_H

#include <stdlib.h>
#include "objTreeNode.h"
#include "debug.h"

namespace PowerAPI {

class RpcTreeNode : public ObjTreeNode {

  public:
    RpcTreeNode( Cntxt* ctx, std::string name, PWR_ObjType type,
            std::string config, ObjTreeNode* parent = NULL ) 
	  : ObjTreeNode( ctx, name, type, parent ) 
    {
        DBGX("name=`%s` config=`%s`\n",name.c_str(), config.c_str());
	    m_url = config.substr(0, config.find_first_of( ":") );
	    m_port = atoi( config.substr( config.find_first_of( ":") + 1, -1 ).c_str() ); 
        DBGX("url=%s %d\n",m_url.c_str(),m_port);
    }

    virtual ~RpcTreeNode() {};

    int attrGetValue( PWR_AttrName attr, void* buf, size_t len, PWR_Time* ts );
    int attrSetValue( PWR_AttrName attr, void* buf, size_t len );
    int attrGetValues( const std::vector<PWR_AttrName>& attrs, void* buf,
                         std::vector<PWR_Time>& ts, std::vector<int>& status );
    int attrSetValues( const std::vector<PWR_AttrName>& attrs, void* buf,
                                            std::vector<int>& status  );

    int attrStartStat( PWR_AttrName, PWR_AttrStat ) {
        return PWR_RET_FAILURE; 
    }
    int attrStopStat( PWR_AttrName, PWR_AttrStat) {
        return PWR_RET_FAILURE; 
    }
    int attrClearStat( PWR_AttrName, PWR_AttrStat) {
        return PWR_RET_FAILURE; 
    }
    int attrGetStat( PWR_AttrName, PWR_AttrStat,
                double* value, PWR_StatTimes* statTimes ) {
        return PWR_RET_FAILURE; 
    }
    int attrGetStats( PWR_AttrName, PWR_AttrStat,
                double* value, PWR_StatTimes* statTimes ) {
        return PWR_RET_FAILURE; 
    }

  private:
    std::string m_url;
    int         m_port;
};

}

#endif
