#ifndef _PWR_CNTXT_h
#define _PWR_CNTXT_h

#include <string>
#include <deque>
#include <map>

#include "treeNode.h"
#include "util.h"
#include "types.h"
#include "dev.h"
#include "config.h"

namespace PowerAPI {

class Grp;
class ObjTreeNode;

class Cntxt {

  public:

    Cntxt( PWR_CntxtType type, PWR_Role role, const char* name );
    ~Cntxt( );

	void initAttr( TreeNode*, TreeNode::AttrEntry& );
	ObjTreeNode* findNode( std::string name );
	Grp* findChildren( ObjTreeNode* );

    ObjTreeNode* getSelf();

    Grp* getGrp( PWR_ObjType type ) {
        Grp* grp = getGrpByName( objTypeToString( type ) ); 
        if ( ! grp ) {
            grp = initGrp( type ); 
            m_groupMap[ objTypeToString( type ) ] = grp;
        }
        return grp; 
    }

    Grp* getGrpByName( std::string name ) {
        if ( m_groupMap.find( name ) == m_groupMap.end() ) {
            return NULL;
        }
        return m_groupMap[name];
    }

    Grp* groupCreate( std::string name );
    int groupDestroy( Grp* grp );

	Config* config() { return m_config; }

  private:
	
    void initPlugins( Config& );
	void finiPlugins( );
    void initDevices( Config& );
	void finiDevices( );
    bool standAlone() { return m_standAlone; } 

    ObjTreeNode* findObject( std::string name );
    
    Grp* initGrp( PWR_ObjType type );
	
    std::string     m_myLocation;
    std::string 	m_configFile;
    ObjTreeNode*    m_top;
	Config*			m_config;
    bool            m_standAlone;

    std::map<std::string,TreeNode*>	m_objTreeNodeMap;
	std::deque< TreeNode* >			m_devTreeNodes;

    std::map< std::string, plugin_dev_t* >     m_pluginLibMap;
    std::map< std::string, Grp* > 			   m_groupMap;
    std::map< std::string, std::pair< plugin_dev_t*, plugin_devops_t* > > m_devMap;
};

}

#endif
