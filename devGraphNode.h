#ifndef _PWR_DEVGRAPHNODE_H
#define _PWR_DEVGRAPHNODE_H

#include <string>

#include "graphNode.h"
#include "dev.h"
#include "debug.h"

class DevGraphNode : public GraphNode {
  public:

    DevGraphNode( pwr_dev_t dev, plugin_dev_t* ops, const std::string config )
	  : m_ops( ops ) 
	{
        DBGX("\n");
        m_fd = m_ops->open( dev, config.c_str() );
    }

    int attrGetValues( const std::vector<PWR_AttrName>& names, void* ptr,
                    std::vector<PWR_Time>& ts, std::vector<int>& status ){
        DBGX("\n");
        return m_ops->readv( m_fd, names.size(), &names[0], ptr,
                            &ts[0], &status[0] );
        return -1;
    }

    int attrSetValues( const std::vector<PWR_AttrName>& names, void* ptr,
                    std::vector<int>& status ){
        DBGX("\n");
        return m_ops->writev( m_fd, names.size(), &names[0], ptr,
                                                            &status[0] );
        return -1;
    }

    int attrGetValue( PWR_AttrName name, void* ptr, size_t len, PWR_Time* ts ){
        DBGX("\n");
        return m_ops->read( m_fd, name, ptr, len, ts );
        return -1;
    }

    int attrSetValue( PWR_AttrName name, void* ptr, size_t len ) {
        DBGX("\n");
        return m_ops->write( m_fd, name, ptr, len );
        return -1;
    }
    ~DevGraphNode() {
        m_ops->close( m_fd );
    }

  private:
    plugin_dev_t*   m_ops;
    pwr_fd_t        m_fd;
};

#endif
