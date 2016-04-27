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

#include <assert.h>
#include <sstream>

#include "debug.h"
#include "util.h"
#include <hwloc.h>

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

	m_root = new TreeNode( NULL, PWR_OBJ_PLATFORM, "plat" );
    print_children(topology, hwloc_get_root_obj(topology), 0, m_root );
	print( m_root );

	initAttributes( m_root );

	assert(0);
	unlock();
}

HwlocConfig::~HwlocConfig()
{
	lock();
	unlock();
}

void HwlocConfig::initAttributes( TreeNode* node )
{
	if ( node->children.size() ) {
		for ( unsigned i = 0; i < node->children.size(); i++ ) {
			initAttributes( node->children[i] );
		}	
	}	
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
	std::deque< Config::Plugin > retval;
	assert(0);

	lock();
	unlock();
	return retval;
}

std::deque< Config::SysDev > HwlocConfig::findSysDevs()
{
	DBGX2(DBG_CONFIG,"\n");
	std::deque< Config::SysDev > retval;
	assert(0);

	lock();
	unlock();
	return retval;
}

std::deque< Config::ObjDev > 
			HwlocConfig::findObjDevs( std::string name, PWR_AttrName attr )
{
	std::deque< Config::ObjDev > devs;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
                            name.c_str(),attrNameToString(attr));

	assert(0);
	lock();
	unlock();
	return devs;
}

std::deque< std::string >
        HwlocConfig::findAttrChildren( std::string name, PWR_AttrName attr )
{
	std::deque< std::string > children;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
                            name.c_str(),attrNameToString(attr));

	assert(0);
	lock();
	unlock();
	return children;
}

std::string HwlocConfig::findAttrType( std::string name, PWR_AttrName attr )
{
	std::string retval;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
							name.c_str(),attrNameToString(attr));

	lock();
	unlock();

	return retval;
}

std::string HwlocConfig::findAttrOp( std::string name, PWR_AttrName attr )
{
	std::string retval;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
							name.c_str(),attrNameToString(attr));
	lock();
	unlock();

	return retval;
}

std::string HwlocConfig::findAttrHz( std::string name, PWR_AttrName attr )
{
	std::string retval;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
							name.c_str(),attrNameToString(attr));
	assert(0);

	lock();
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
void HwlocConfig::print( TreeNode* node )
{
	DBGX2(DBG_CONFIG,"%s %s\n",
					getFullName(node).c_str(), objTypeToString(node->type) ); 
	if ( node->children.empty() ) {
		return;
	} 

	for ( unsigned i = 0; i < node->children.size(); i++ ) {
		print( node->children[i] );
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
		case HWLOC_OBJ_NODE: return PWR_OBJ_NODE;
		case HWLOC_OBJ_SOCKET: return PWR_OBJ_SOCKET;
		case HWLOC_OBJ_CORE: return PWR_OBJ_CORE;
		default: return PWR_OBJ_INVALID;
	}
}

static std::string getName( hwloc_obj_type_t type ) {
	switch ( type ) {
		case HWLOC_OBJ_NODE: return "node";
		case HWLOC_OBJ_SOCKET: return "socket";
		case HWLOC_OBJ_CORE: return "core";
		default: return "";
	}
}

void HwlocConfig::print_children(hwloc_topology_t topology, hwloc_obj_t obj, 
                           int depth, TreeNode* parent )
{
    unsigned i;
	switch( obj->type ) {
		case HWLOC_OBJ_NODE:
		case HWLOC_OBJ_SOCKET:
		case HWLOC_OBJ_CORE:
    		//hwloc_obj_snprintf(string, sizeof(string), topology, obj, "#", 0);
    		//printf("%*s%s\n", 2*depth, "", string);
			{	
				std::stringstream tmp;
				tmp << getName(obj->type ) << obj->logical_index;
				TreeNode* me = new TreeNode( parent,  
					convertType( obj->type ), tmp.str() );
				parent->children.push_back(me);
				parent = me;
			}
			break;		
		default:
			break;
	}
    for (i = 0; i < obj->arity; i++) {
        print_children(topology, obj->children[i], depth + 1,  parent );
    }
}

