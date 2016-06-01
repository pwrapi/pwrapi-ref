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

#include "hwlocConfig.h"

#include <sys/utsname.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <hwloc.h>

#include "debug.h"
#include "util.h"

using namespace PowerAPI;

pthread_mutex_t HwlocConfig::m_mutex = PTHREAD_MUTEX_INITIALIZER;

HwlocConfig::HwlocConfig( std::string file ) 
{
	DBGX2(DBG_CONFIG,"config file `%s`\n",file.c_str());

	lock();

    hwloc_topology_t topology;
    /* Allocate and initialize topology object. */
    hwloc_topology_init(&topology);
    /* ... Optionally, put detection configuration here to ignore
       some objects types, define a synthetic topology, etc....  
       The default is to detect all the objects of the machine that
       the caller is allowed to access.  See Configure Topology
       Detection. */
    /* Perform the topology detection. */
    hwloc_topology_load(topology);

	m_root = new TreeNode( NULL, PWR_OBJ_PLATFORM, "plat", 0 );
	assert(m_root);
    initHierarchy( hwloc_get_root_obj(topology), m_root );
	printTree( m_root );

	std::string line;
	std::ifstream config;

	config.open( file.c_str() );

   	struct utsname name;
   	int rc = uname( &name );
    assert( 0 == rc );
    std::string os( name.sysname);

	std::string ext;
    if ( 0 == os.compare("Darwin") ) {
    	ext = ".dylib";
    } else {
        ext = ".so";
    }

	while ( getline(config,line) ) {
		Config::Plugin tmp;


		tmp.lib = line;
		PluginMeta* meta = new PluginMeta( tmp.lib + ext );
		assert(meta);

		tmp.name = meta->getPluginName();

		m_meta.push_back(meta);
		m_libs.push_back(tmp);
		DBGX2(DBG_CONFIG,"lib='%s' name='%s'\n",
				tmp.lib.c_str(),tmp.name.c_str());
	}
	config.close();

	reorderObjects( m_root );

	pruneObjects( m_root, m_meta );
	initAttributes( m_root, m_meta );

	unlock();
}

HwlocConfig::~HwlocConfig()
{
	lock();
	unlock();
}

void HwlocConfig::initAttributes( TreeNode* node, std::deque<PluginMeta*>& meta )
{
	for ( unsigned i = 0; i < node->children.size(); i++ ) {
		initAttributes( node->children[i], meta );
	}	
	DBGX2(DBG_CONFIG,"%s\n",getFullName(node).c_str());
	for ( int i = 0; i < PWR_NUM_ATTR_NAMES; i++ ) {
		std::deque<PluginMeta*>::iterator iter =  meta.begin();
		for ( ; iter != meta.end(); ++iter ) {
			if ( (*iter)->findAttr( node->type, (PWR_AttrName) i ) ) {
				DBGX2(DBG_CONFIG,"leaf %s\n",attrNameToString((PWR_AttrName)i));
				Attr attr;
				attr.op = "SUM";
				attr.type = "Float";
				attr.hz = "1";

		    	attr.device = (*iter)->getDevName(node->type);
				attr.openString = (*iter)->getDevOpenStr( node->type, node->global_index );
            	m_devices[ attr.device ] = *iter; 

				node->attrs[(PWR_AttrName)i] = attr;
			} else if ( canAggregate( (PWR_AttrName)i)  ) {

				if ( ! node->children.empty() ) {
					if ( node->children[0]->attrs.find( (PWR_AttrName)i ) !=
						node->children[0]->attrs.end() ) 
					{
						DBGX2(DBG_CONFIG,"inner %s\n",attrNameToString((PWR_AttrName)i));
						Attr attr;
						attr.op = "SUM";
						attr.type = "Float";
						attr.hz = "1";
						node->attrs[(PWR_AttrName)i] = attr;
					}
				}
			}
		}
	}
}

std::vector<HwlocConfig::TreeNode*> HwlocConfig::reorderObjects( TreeNode* node )
{
    std::vector<TreeNode*> ret;
    if ( node->type == PWR_OBJ_MEM ) {
        ret = node->children;
        node->children.clear();
        return ret;
    }

    for ( unsigned i = 0; i < node->children.size(); i++ ) {
        std::vector<TreeNode*> tmp = reorderObjects( node->children[i] );
        ret.insert( ret.end(), tmp.begin(), tmp.end() );
        for ( unsigned i = 0; i < tmp.size(); i++ ) {
            tmp[i]->parent = node;
        } 
    } 
    node->children.insert( node->children.end(), ret.begin(), ret.end() );
    ret.clear();
    return ret;
}


bool HwlocConfig::pruneObjects( TreeNode* node, std::deque<PluginMeta*>& meta )
{
	if ( node->children.size() ) {
        std::vector< TreeNode* >::iterator iter = node->children.begin();
		for ( ; iter != node->children.end(); ) {
			bool ret = pruneObjects( *iter, meta );
            if ( ! ret ) {
	            DBGX2(DBG_CONFIG,"prune %s\n",getFullName(*iter).c_str());
                delete *iter;
                iter = node->children.erase(iter);
            }else {
                ++iter;
            }
		}	
    }
    
	//DBGX2(DBG_CONFIG,"%s\n",getFullName(node).c_str());

    return isSupported( node, meta ); 
}

bool HwlocConfig::isSupported( TreeNode* node, std::deque<PluginMeta*>& meta )
{

	bool ret = false;
	// make sure to plugins don't support the same thin
	for ( int attr = 0; attr < PWR_NUM_ATTR_NAMES; attr++ ) { 
		int num = 0; 
		std::deque<PluginMeta*>::iterator iter = meta.begin();
		for ( ; iter != meta.end(); ++iter ) {
	    	if ( (*iter)->isObjectSupported( node->type, (PWR_AttrName)attr ) ) 
			{
	        	DBGX2(DBG_CONFIG,"supported %s\n",getFullName(node).c_str());
				++num;
				ret = true;
			}
		}
		assert( num < 2 );
	}


	return ( ret || ! node->children.empty() );
}

bool HwlocConfig::hasServer( const std::string name ) 
{
	DBGX2(DBG_CONFIG,"find %s\n",name.c_str());
	return false;
}

bool HwlocConfig::hasObject( const std::string name ) 
{
	DBGX2(DBG_CONFIG,"find %s\n",name.c_str());
	assert(0);

	lock();
	unlock();
	return false;
}

PWR_ObjType HwlocConfig::objType( const std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());

	PWR_ObjType type = PWR_OBJ_INVALID;

	lock();
	TreeNode* node = findObj( m_root, name );
	if ( node ) {
		type = node->type;
	}
	unlock();

	return type;
}

std::deque< Config::Plugin > HwlocConfig::findPlugins( )
{
	DBGX2(DBG_CONFIG,"\n");
	return m_libs;
}

std::deque< Config::SysDev > HwlocConfig::findSysDevs()
{
	DBGX2(DBG_CONFIG,"\n");
	std::deque< Config::SysDev > retval;

	lock();
	std::map<std::string,PluginMeta*>::iterator iter = m_devices.begin(); 

	for ( ; iter != m_devices.end(); ++iter ) {
		Config::SysDev tmp;
		tmp.name = iter->first;
		tmp.plugin = iter->second->getPluginName();
        tmp.initString = iter->second->getDevInitStr(iter->first); 
		retval.push_back( tmp );
	}
	unlock();
	return retval;
}

std::deque< Config::ObjDev > 
			HwlocConfig::findObjDevs( std::string name, PWR_AttrName attr )
{
	std::deque< Config::ObjDev > devs;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
                            name.c_str(),attrNameToString(attr));

#if 0
	printf("%s obj=`%s` attr=`%s`\n", __func__,
                            name.c_str(),attrNameToString(attr));
#endif
	lock();
	TreeNode* node = findObj(m_root, name); 
	if ( node->attrs.find( attr ) != node->attrs.end() ) {
		if ( ! node->attrs[attr].device.empty() ) { 
			Config::ObjDev dev;
			dev.device = node->attrs[attr].device;
			dev.openString = node->attrs[attr].openString;
			devs.push_back(dev);
		}
	}	
	unlock();
	return devs;
}

std::deque< std::string >
        HwlocConfig::findAttrChildren( std::string name, PWR_AttrName attr )
{
	std::deque< std::string > children;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
                            name.c_str(),attrNameToString(attr));

	lock();
	TreeNode* node = findObj(m_root, name); 
	for ( unsigned i = 0; i < node->children.size(); i++ ) {
        // check to see if child was pruned
        if ( node->children[i] ) {
		    children.push_back( getFullName( node->children[i] ) );
        }
	}
	unlock();
	return children;
}

std::string HwlocConfig::findAttrType( std::string name, PWR_AttrName attr )
{
	std::string retval;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
							name.c_str(),attrNameToString(attr));

	lock();
	TreeNode* node = findObj(m_root, name); 
	if ( node->attrs.find( attr ) != node->attrs.end() ) {
		retval = node->attrs[attr].type;
	}	
	unlock();

	return retval;
}

std::string HwlocConfig::findAttrOp( std::string name, PWR_AttrName attr )
{
	std::string retval;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
							name.c_str(),attrNameToString(attr));
	lock();
	TreeNode* node = findObj(m_root, name); 
	if ( node->attrs.find( attr ) != node->attrs.end() ) {
		retval = node->attrs[attr].op;
	}	
	unlock();

	return retval;
}

std::string HwlocConfig::findAttrHz( std::string name, PWR_AttrName attr )
{
	std::string retval;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
							name.c_str(),attrNameToString(attr));

	lock();
	TreeNode* node = findObj(m_root, name); 
	if ( node->attrs.find( attr ) != node->attrs.end() ) {
		retval = node->attrs[attr].hz;
	}	
	unlock();

	return retval;
}

std::deque< std::string > HwlocConfig::findChildren( std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());
	std::deque< std::string > children;

	lock();

	TreeNode* node = findObj( m_root, name );

	if ( node ) {
		for ( unsigned i = 0; i < node->children.size(); i++ ) {
			children.push_back( getFullName( node->children[i] ) );
		}
	}

	unlock();

	return children;
}

std::string HwlocConfig::findParent( std::string name )
{
	std::string retval;

	lock();
	TreeNode* node = findObj( m_root, name );
	if ( node && node->parent) {
		retval = getFullName( node->parent );
	}
	unlock();
	DBGX2(DBG_CONFIG,"%s %s\n",name.c_str(),retval.c_str());
	return retval;
}

HwlocConfig::TreeNode* HwlocConfig::findObj( TreeNode* node, std::string name )
{
	DBGX2(DBG_CONFIG,"name=%s type=%s\n", 
				getFullName(node).c_str(), objTypeToString(node->type) ); 

	if ( ! getFullName( node ).compare(name) ) {
		return node;
	} 

	for ( unsigned i = 0; i < node->children.size(); i++ ) {
		TreeNode *tmp;
		if ( ( tmp = findObj( node->children[i], name ) ) ) {
			return tmp;
		}
	}
	return NULL;	
}
void HwlocConfig::printTree( TreeNode* node )
{
	DBGX2(DBG_CONFIG,"%s %s\n",
					getFullName(node).c_str(), objTypeToString(node->type) ); 
	if ( node->children.empty() ) {
		return;
	} 

	for ( unsigned i = 0; i < node->children.size(); i++ ) {
		printTree( node->children[i] );
	}
}

std::string HwlocConfig::getFullName( TreeNode* node )
{
	std::string prefix;	
	if ( node->parent ) {
		prefix = getFullName( node->parent );
		prefix += ".";
	}
	return  prefix  + node->name;
}
static PWR_ObjType convertType( hwloc_obj_type_t type ) {
	switch ( type ) {
		case HWLOC_OBJ_NODE: return PWR_OBJ_MEM;
		case HWLOC_OBJ_MACHINE: return PWR_OBJ_NODE;
		case HWLOC_OBJ_SOCKET: return PWR_OBJ_SOCKET;
		case HWLOC_OBJ_CORE: return PWR_OBJ_CORE;
		default: return PWR_OBJ_INVALID;
	}
}

static std::string getName( hwloc_obj_type_t type ) {
	switch ( type ) {
		case HWLOC_OBJ_MACHINE: return "node";
		case HWLOC_OBJ_NODE: return "mem";
		case HWLOC_OBJ_SOCKET: return "socket";
		case HWLOC_OBJ_CORE: return "core";
		default: return "";
	}
}

void HwlocConfig::initHierarchy( hwloc_obj_t obj, TreeNode* parent )
{
#if 0
    printf("%s parent->%s %lu \n",hwloc_obj_type_string(obj->type),
                ! obj->parent ? "":
                hwloc_obj_type_string(obj->parent->type),
                obj->memory.total_memory);
#endif
    unsigned i;
	switch( obj->type ) {
		case HWLOC_OBJ_MACHINE:
		case HWLOC_OBJ_NODE:
		case HWLOC_OBJ_SOCKET:
		case HWLOC_OBJ_CORE:
			{	
				std::stringstream tmp;
				tmp << getName(obj->type ) << obj->os_index;
				TreeNode* me = new TreeNode( parent,  
					convertType( obj->type ), tmp.str(), obj->logical_index );
				parent->children.push_back(me);
				parent = me;
			}
			break;		
		default:
			break;
	}
    for (i = 0; i < obj->arity; i++) {
        initHierarchy( obj->children[i], parent );
    }
}

