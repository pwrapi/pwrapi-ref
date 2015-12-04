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

#include <assert.h>

#include "pyConfig.h"
#include "debug.h"

using namespace PowerAPI;

PyConfig::PyConfig( std::string file ) 
{
	DBGX2(DBG_CONFIG,"config file `%s`\n",file.c_str());
	Py_Initialize();
	
	//m_pName = PyString_FromString( (char*) file.c_str() );
	m_pName = PyString_FromString( "xtpm-node" );
	assert( m_pName );

	m_pModule = PyImport_Import( m_pName );
	assert( m_pModule );

	Py_DECREF( m_pName );
}

PyConfig::~PyConfig()
{
	Py_Finalize();
}

void PyConfig::print( std::ostream& out  )
{
}

bool PyConfig::hasObject( const std::string name ) 
{
	DBGX2(DBG_CONFIG,"find %s\n",name.c_str());

	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "hstObject" );
	assert(pFunc);
	PyObject* pArgs = PyTuple_New( 1 );
	assert(pArgs);

	PyObject* pValue = PyInt_FromLong( 0 );
	assert(pValue);

	PyTuple_SetItem( pArgs, 0, pValue );
	
	pValue = PyObject_CallObject( pFunc, pArgs );
	assert(pValue);
	assert(0);
}

PWR_ObjType PyConfig::objType( const std::string name )
{
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "getObjType" );
	assert(pFunc);
	PyObject* pArgs = PyTuple_New( 1 );
	assert(pArgs);

	PyObject* pValue = PyString_FromString( name.c_str() );
	assert(pValue);

	PyTuple_SetItem( pArgs, 0, pValue );
	
	pValue = PyObject_CallObject( pFunc, pArgs );

	assert(pValue);
	char* tmp = PyString_AsString(pValue);

	DBGX2(DBG_CONFIG,"obj=`%s` type=%s\n",name.c_str(),tmp);
	return objTypeStrToInt( tmp );
}

std::deque< Config::Plugin > PyConfig::findPlugins( )
{
	DBGX2(DBG_CONFIG,"\n");
	std::deque< Config::Plugin > retval;

	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findPlugins" );
	assert(pFunc);

	PyObject* pValue = PyObject_CallObject( pFunc, NULL );

	//PyObject_Print( pValue, stderr, Py_PRINT_RAW ); printf("\n");
	for ( int i; i < PyList_Size( pValue); i++ ) {

		PyObject* tmp = PyList_GetItem(pValue,i);

		assert( 2 == PyList_Size(tmp ) );
		Config::Plugin plugin;
		plugin.name = PyString_AsString(PyList_GetItem(tmp,0) );
		plugin.lib = PyString_AsString(PyList_GetItem(tmp,1) );
		DBGX2(DBG_CONFIG,"%s %s \n", plugin.name.c_str(), plugin.lib.c_str() );
		
		retval.push_back( plugin ); 
	}
	assert(pValue);
	return retval;
}

std::deque< Config::SysDev > PyConfig::findSysDevs()
{
	DBGX2(DBG_CONFIG,"\n");
	std::deque< Config::SysDev > retval;

	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findSysDevs" );
	assert(pFunc);

	PyObject* pValue = PyObject_CallObject( pFunc, NULL );

	//PyObject_Print( pValue, stderr, Py_PRINT_RAW ); printf("\n");
	for ( int i; i < PyList_Size( pValue); i++ ) {

		PyObject* tmp = PyList_GetItem(pValue,i);

		assert( 3 == PyList_Size(tmp ) );
		Config::SysDev tmp2;
		tmp2.name = PyString_AsString(PyList_GetItem(tmp,0) );
		tmp2.plugin = PyString_AsString(PyList_GetItem(tmp,1) );
		tmp2.initString = PyString_AsString(PyList_GetItem(tmp,2) );
		DBGX2(DBG_CONFIG,"%s %s \n", tmp2.name.c_str(), 
			tmp2.plugin.c_str(), tmp2.initString.c_str() );
		
		retval.push_back( tmp2 ); 
	}
	assert(pValue);
	return retval;
}

std::deque< Config::ObjDev > 
			PyConfig::findObjDevs( std::string name, PWR_AttrName attr )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());
	std::deque< Config::ObjDev > devs;

	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findObjDevs" );
	assert(pFunc);
	PyObject* pArgs = PyTuple_New( 2 );
	assert(pArgs);

	PyObject* pValue = PyString_FromString( name.c_str() );
	assert(pValue);

	PyTuple_SetItem( pArgs, 0, pValue );

	pValue = PyString_FromString( attrNameToString(attr).c_str() );
	assert(pValue);
	PyTuple_SetItem( pArgs, 1, pValue );
	
	pValue = PyObject_CallObject( pFunc, pArgs );

	//PyObject_Print( pValue, stderr, Py_PRINT_RAW ); printf("\n");
	for ( int i; i < PyList_Size( pValue); i++ ) {

		PyObject* tmp = PyList_GetItem(pValue,i);

		assert( 2 == PyList_Size(tmp ) );
		Config::ObjDev dev;
		dev.device = PyString_AsString(PyList_GetItem(tmp,0) );
		dev.openString = PyString_AsString(PyList_GetItem(tmp,1) );
		DBGX2(DBG_CONFIG,"%s %s \n", dev.device.c_str(), dev.openString.c_str() );
		
		devs.push_back( dev ); 
	}
	assert(pValue);
	return devs;
}

std::deque< std::string >
        PyConfig::findAttrChildren( std::string name, PWR_AttrName attr )
{
	std::deque< std::string > children;
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());

	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findAttrChildren" );
	assert(pFunc);

	PyObject* pArgs = PyTuple_New( 2 );
	assert(pArgs);

	PyObject* pValue = PyString_FromString( name.c_str() );
	assert(pValue);

	PyTuple_SetItem( pArgs, 0, pValue );

	pValue = PyString_FromString( attrNameToString(attr).c_str() );
	assert(pValue);
	PyTuple_SetItem( pArgs, 1, pValue );
	
	pValue = PyObject_CallObject( pFunc, pArgs );
	assert(pValue);

	for ( int i; i < PyList_Size( pValue); i++ ) {
		char* str = PyString_AsString(PyList_GetItem(pValue,i) );
		DBGX2(DBG_CONFIG,"%s \n", str );
		children.push_back( str ); 
	}

	return children;
}

std::string PyConfig::findAttrOp( std::string name, PWR_AttrName attr )
{
	assert(0);
}

std::deque< std::string > PyConfig::findChildren( std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());
	std::deque< std::string > children;

	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findChildren" );
	assert(pFunc);
	PyObject* pArgs = PyTuple_New( 1 );
	assert(pArgs);

	PyObject* pValue = PyString_FromString( name.c_str() );
	assert(pValue);

	PyTuple_SetItem( pArgs, 0, pValue );
	
	pValue = PyObject_CallObject( pFunc, pArgs );

	//PyObject_Print( pValue, stderr, Py_PRINT_RAW );
	for ( int i; i < PyList_Size( pValue); i++ ) {
		char* str = PyString_AsString(PyList_GetItem(pValue,i) );
		DBGX2(DBG_CONFIG,"%s \n", str );
		children.push_back( str ); 
	}
	assert(pValue);
	return children;
}

Config::Location PyConfig::findLocation( std::string name )
{
	assert(0);
}

std::string PyConfig::findParent( std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());
	assert(0);
}

std::string PyConfig::findObjLocation( std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());
	return "";
}

std::string PyConfig::objTypeToString( PWR_ObjType type )
{
    switch( type ) {
    case PWR_OBJ_PLATFORM: return "Platform";
    case PWR_OBJ_CABINET:  return "Cabinet";
    case PWR_OBJ_BOARD:    return "Board";
    case PWR_OBJ_NODE:     return "Node";
    case PWR_OBJ_SOCKET:   return "Socket";
    case PWR_OBJ_CORE:     return "Core";
    case PWR_OBJ_NIC:      return "Nic";
    case PWR_OBJ_MEM:      return "Memory";
    case PWR_OBJ_INVALID:  return "Invalid";
    }
    return NULL;
}

PWR_ObjType PyConfig::objTypeStrToInt( const std::string name )
{
    if ( 0 == name.compare( "Platform" ) ) {
        return  PWR_OBJ_PLATFORM;
    } else if ( 0 == name.compare( "Cabinet" ) ) {
        return PWR_OBJ_CABINET;
    } else if ( 0 == name.compare( "Board" ) ) {
        return PWR_OBJ_BOARD;
    } else if ( 0 == name.compare( "Node" ) ) {
        return PWR_OBJ_NODE;
    } else if ( 0 == name.compare( "Socket" ) ) {
        return PWR_OBJ_SOCKET;
    } else if ( 0 == name.compare( "Core" ) ) {
        return PWR_OBJ_CORE;
    }
    return PWR_OBJ_INVALID;
}

std::string PyConfig::attrNameToString( PWR_AttrName name )
{
    switch( name ){
    case PWR_ATTR_NAME: return "Name";
    case PWR_ATTR_FREQ: return "FREQ";
    case PWR_ATTR_TEMP: return "TEMP";
    case PWR_ATTR_PSTATE: return "PSTATE";
    case PWR_ATTR_MAX_POWER: return "MAX_POWER";
    case PWR_ATTR_MIN_POWER: return "MIN_POWER";
    case PWR_ATTR_AVG_POWER: return "AVG_POWER";
    case PWR_ATTR_POWER: return "POWER";
    case PWR_ATTR_VOLTAGE: return "VOLTAGE";
    case PWR_ATTR_CURRENT: return "CURRENT";
    case PWR_ATTR_ENERGY: return "ENERGY";
    case PWR_ATTR_INVALID: return "Invalid";
    default: return "????";
    }
    return NULL;
}
