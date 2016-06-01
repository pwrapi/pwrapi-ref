/* 
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#ifndef _PWR_HWLOCCONFIG_H
#define _PWR_HWLOCCONFIG_H

#include <map>
#include <hwloc.h>
#include <pthread.h>
#include "pwrtypes.h"
#include "config.h"
#include "pluginMeta.h"

#include <vector>


namespace PowerAPI {

class HwlocConfig : public Config {
	struct Attr {
		std::string op;
		std::string hz;
		std::string type;
		std::string device;
		std::string openString;
	};

	struct TreeNode {
		TreeNode( TreeNode* _parent, PWR_ObjType _type,
				std::string _name, unsigned _global_index ) : 
				parent(_parent), type(_type), name(_name), 
				global_index( _global_index )  {}
		TreeNode* parent;
		PWR_ObjType type;
		std::string name;
		unsigned global_index;
		std::map<PWR_AttrName,Attr> attrs;
		std::vector<TreeNode*> children;
	};
  public:

	HwlocConfig( std::string file );
	~HwlocConfig();
	
    std::string findParent( std::string name );
	std::deque< std::string >
                findAttrChildren( std::string, PWR_AttrName );
	std::string findAttrOp( std::string, PWR_AttrName );
	std::string findAttrHz( std::string, PWR_AttrName );
	std::string findAttrType( std::string, PWR_AttrName );
	std::deque< std::string > findChildren( std::string );
	std::deque< Config::ObjDev > findObjDevs( std::string, PWR_AttrName );
	std::deque< Config::Plugin > findPlugins();
	std::deque< Config::SysDev > findSysDevs();
	bool hasServer( std::string );
	bool hasObject( const std::string name );
	PWR_ObjType objType( const std::string );

  private:

	std::deque< PluginMeta* > m_meta;
	void initAttributes( TreeNode*, std::deque<PluginMeta*>& );
    std::vector<TreeNode*> reorderObjects( TreeNode* );
    bool pruneObjects( TreeNode*, std::deque<PluginMeta*>& );
    bool isSupported( TreeNode*, std::deque<PluginMeta*>& );

	TreeNode* findObj( TreeNode*, std::string );
	std::string getFullName( TreeNode* node ); 
	void lock() {
		pthread_mutex_lock(&m_mutex);
		//printf("locked %p\n", pthread_self());
	}

	void unlock() {
		pthread_mutex_unlock(&m_mutex);
		//printf("unlocked %p\n",pthread_self());
	}

	void initHierarchy( hwloc_obj_t obj, TreeNode* parent );
	void printTree( TreeNode* );
	TreeNode*	m_root;
    static pthread_mutex_t	m_mutex;
	std::deque< Config::Plugin > m_libs;
    std::map< std::string, PluginMeta* > m_devices;
};

}

#endif
