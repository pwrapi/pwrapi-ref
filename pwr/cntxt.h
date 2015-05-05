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

#ifndef _PWR_CNTXT_h
#define _PWR_CNTXT_h

#include <string>
#include <deque>
#include <map>

#include "treeNode.h"
#include "util.h"
#include "pwrtypes.h"
#include "dev.h"
#include "config.h"

namespace PowerAPI {

class Grp;
class ObjTreeNode;
class Stat;

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

    plugin_dev_t* getDev( std::string lib, std::string name );

    Grp* getGrpByName( std::string name ) {
        if ( m_groupMap.find( name ) == m_groupMap.end() ) {
            return NULL;
        }
        return m_groupMap[name];
    }

    Grp* groupCreate( std::string name );
    int groupDestroy( Grp* grp );

	Stat* createStat( ObjTreeNode*, PWR_AttrName, PWR_AttrStat );
	Stat* createStat( Grp*, PWR_AttrName, PWR_AttrStat );
	int destroyStat( Stat* );

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
