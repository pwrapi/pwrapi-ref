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
        DBGX("%s\n",config.c_str());
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

  private:
    std::string m_url;
    int         m_port;
};

}

#endif
