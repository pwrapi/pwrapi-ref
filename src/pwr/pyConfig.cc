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

#include "pyConfig.h"

#include <assert.h>

#include "debug.h"

using namespace PowerAPI;

PyObject* PyConfig::m_pModule = NULL;

pthread_mutex_t PyConfig::m_mutex = PTHREAD_MUTEX_INITIALIZER;

PyConfig::PyConfig( std::string file ) 
{
	DBGX2(DBG_CONFIG,"config file `%s`\n",file.c_str());

	size_t pos = file.find_last_of('/');

	std::string path;
	std::string module = file;
	if ( std::string::npos != pos ) { 
 		path = file.substr( 0, pos );
		module = file.substr( pos + 1 );

		setenv( "PYTHONPATH", path.c_str(), 1 );
	}
	module = module.substr(0, module.find_last_of('.' ) );

	DBGX2( DBG_CONFIG, "path=`%s` module=`%s`\n",
									path.c_str(), module.c_str() );

	lock();
	if ( NULL == m_pModule ) {

		Py_Initialize();
	
		m_pModule = PyImport_ImportModule( module.c_str() );
		assert( m_pModule );
	}
	unlock();
}

PyConfig::~PyConfig()
{
	lock();
#if 0
this is causing this error
Exception KeyError: KeyError(46912551089920,) in <module 'threading' from '/usr/lib64/python2.6/threading.pyc'> ignored
	if ( m_pModule ) {
		Py_DECREF( m_pModule );
		Py_Finalize();
		m_pModule = NULL;
	}
#endif
	unlock();
}

void PyConfig::print( std::ostream& out  )
{
}

bool PyConfig::hasServer( const std::string name ) 
{
	DBGX2(DBG_CONFIG,"find %s\n",name.c_str());

	return ! findObjLocation( name ).empty();
}

bool PyConfig::hasObject( const std::string name ) 
{
	DBGX2(DBG_CONFIG,"find %s\n",name.c_str());

	lock();
	// new reference
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "hasObject" );
	assert(pFunc);

	// new reference	
	PyObject* pArgs = PyTuple_New( 1 );
	assert(pArgs);

	PyTuple_SetItem( pArgs, 0, PyInt_FromLong( 0 ) );
	
	// new referenc
	PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );
	assert(pRetval);

	Py_DECREF( pFunc );
	Py_DECREF( pArgs );
	Py_DECREF( pRetval);
	unlock();
	assert(0);
}

PWR_ObjType PyConfig::objType( const std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());

	PWR_ObjType type = PWR_OBJ_INVALID;

	lock();
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "getObjType" );
	assert(pFunc);

	PyObject* pArgs = PyTuple_New( 1 );
	assert(pArgs);

	PyTuple_SetItem( pArgs, 0, PyString_FromString( name.c_str() ) );

	PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );

	if ( pRetval ) {
		type = objTypeStrToInt( PyString_AsString(pRetval) );
		Py_DECREF(pRetval);
	}
	DBGX2(DBG_CONFIG,"obj=`%s` type=%s\n",
                    name.c_str(),objTypeToString(type).c_str());

	Py_DECREF(pFunc);
	Py_DECREF(pArgs);
	unlock();

	return type;
}

std::deque< Config::Plugin > PyConfig::findPlugins( )
{
	DBGX2(DBG_CONFIG,"\n");
	std::deque< Config::Plugin > retval;

	lock();
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findPlugins" );
	assert(pFunc);

	PyObject* pRetval = PyObject_CallObject( pFunc, NULL );
	assert(pRetval);

	for ( int i=0; i < PyList_Size( pRetval); i++ ) {

		PyObject* tmp = PyList_GetItem(pRetval,i);

		assert( 2 == PyList_Size(tmp ) );
		Config::Plugin plugin;
		plugin.name = PyString_AsString(PyList_GetItem(tmp,0) );
		plugin.lib = PyString_AsString(PyList_GetItem(tmp,1) );
		DBGX2(DBG_CONFIG,"%s %s \n", plugin.name.c_str(), plugin.lib.c_str() );

		Py_DECREF( tmp );
		
		retval.push_back( plugin ); 
	}
	Py_DECREF(pFunc);
	Py_DECREF(pRetval);
	unlock();
	return retval;
}

std::deque< Config::SysDev > PyConfig::findSysDevs()
{
	DBGX2(DBG_CONFIG,"\n");
	std::deque< Config::SysDev > retval;

	lock();
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findSysDevs" );
	assert(pFunc);

	PyObject* pRetval = PyObject_CallObject( pFunc, NULL );
	assert(pRetval);

	for ( int i=0; i < PyList_Size( pRetval ); i++ ) {

		PyObject* tmp = PyList_GetItem(pRetval,i);

		assert( 3 == PyList_Size(tmp ) );
		Config::SysDev tmp2;
		tmp2.name = PyString_AsString(PyList_GetItem(tmp,0) );
		tmp2.plugin = PyString_AsString(PyList_GetItem(tmp,1) );
		tmp2.initString = PyString_AsString(PyList_GetItem(tmp,2) );
		DBGX2(DBG_CONFIG,"%s %s %s\n", tmp2.name.c_str(), 
			tmp2.plugin.c_str(), tmp2.initString.c_str() );

		Py_DECREF( tmp );	
		
		retval.push_back( tmp2 ); 
	}
	Py_DECREF(pFunc);
	Py_DECREF(pRetval);
	unlock();
	return retval;
}

std::deque< Config::ObjDev > 
			PyConfig::findObjDevs( std::string name, PWR_AttrName attr )
{
	std::deque< Config::ObjDev > devs;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
                            name.c_str(),attrNameToString(attr).c_str());

	lock();
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findAttrDevs" );
	assert(pFunc);

	PyObject* pArgs = PyTuple_New( 2 );
	assert(pArgs);

	PyTuple_SetItem( pArgs, 0, PyString_FromString( name.c_str() ) );

	PyTuple_SetItem( pArgs, 1, 
				PyString_FromString( attrNameToString(attr).c_str() ) );
	
	PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );
	assert(pRetval);

	//PyObject_Print( pRetval, stderr, Py_PRINT_RAW ); printf("\n");
	
	for ( int i=0; i < PyList_Size(pRetval); i++ ) {

		PyObject* tmp = PyList_GetItem(pRetval,i);

		assert( 2 == PyList_Size(tmp ) );
		Config::ObjDev dev;
		dev.device = PyString_AsString(PyList_GetItem(tmp,0) );
		dev.openString = PyString_AsString(PyList_GetItem(tmp,1) );
		DBGX2(DBG_CONFIG,"%s %s \n", dev.device.c_str(),
										dev.openString.c_str() );

		Py_DECREF( tmp );
		
		devs.push_back( dev ); 
	}
	//PyObject_Print( pRetval, stderr, Py_PRINT_RAW ); printf("\n");
	Py_DECREF( pRetval );
	Py_DECREF( pFunc );
	Py_DECREF( pArgs );
	unlock();
	return devs;
}

std::deque< std::string >
        PyConfig::findAttrChildren( std::string name, PWR_AttrName attr )
{
	std::deque< std::string > children;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
                            name.c_str(),attrNameToString(attr).c_str());

	lock();
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findAttrChildren" );
	assert(pFunc);

	PyObject* pArgs = PyTuple_New( 2 );
	assert(pArgs);

	PyTuple_SetItem( pArgs, 0, PyString_FromString( name.c_str() ) );

	PyTuple_SetItem( pArgs, 1, 
			PyString_FromString( attrNameToString(attr).c_str() ) );
	
	PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );
	assert(pRetval);

	for ( int i=0; i < PyList_Size(pRetval); i++ ) {
		char* str = PyString_AsString(PyList_GetItem(pRetval,i) );
		DBGX2(DBG_CONFIG,"%s \n", str );
		children.push_back( str ); 
	}

	Py_DECREF( pFunc );
	Py_DECREF( pArgs );
	Py_DECREF( pRetval );
	unlock();
	return children;
}

std::string PyConfig::findAttrType( std::string name, PWR_AttrName attr )
{
	std::string retval;
	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s`\n",
							name.c_str(),attrNameToString(attr).c_str());

	lock();
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findAttrType" );
	assert(pFunc);

	PyObject* pArgs = PyTuple_New( 2 );
	assert(pArgs);

	PyTuple_SetItem( pArgs, 0, PyString_FromString( name.c_str() ) );

	PyTuple_SetItem( pArgs, 1, 
				PyString_FromString( attrNameToString(attr).c_str() ) );
	
	PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );
	assert(pRetval);

	retval = PyString_AsString(pRetval );

	DBGX2(DBG_CONFIG,"'%s'\n", retval.c_str() );

	Py_DECREF( pRetval );
	Py_DECREF( pFunc );
	Py_DECREF( pArgs );
	unlock();

	return retval;
}

std::string PyConfig::findAttrOp( std::string name, PWR_AttrName attr )
{
	std::string retval;

	lock();
	// new
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findAttrOp" );
	assert(pFunc);

	// new
	PyObject* pArgs = PyTuple_New( 2 );
	assert(pArgs);

	// steals 
	PyTuple_SetItem( pArgs, 0, PyString_FromString( name.c_str() ) );

	// steals 
	PyTuple_SetItem( pArgs, 1, 
			PyString_FromString( attrNameToString(attr).c_str() ) );

	// new	
	PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );
	assert(pRetval);

	retval = PyString_AsString( pRetval );

	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s` op=`%s`\n",
				name.c_str(),attrNameToString(attr).c_str(), retval.c_str() );

	Py_DECREF( pRetval );
	Py_DECREF( pFunc );
	Py_DECREF( pArgs );
	unlock();

	return retval;
}

std::string PyConfig::findAttrHz( std::string name, PWR_AttrName attr )
{
	std::string retval;

	lock();
	// new
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findAttrHz" );
	assert(pFunc);

	// new
	PyObject* pArgs = PyTuple_New( 2 );
	assert(pArgs);

	// steals 
	PyTuple_SetItem( pArgs, 0, PyString_FromString( name.c_str() ) );

	// steals 
	PyTuple_SetItem( pArgs, 1, 
			PyString_FromString( attrNameToString(attr).c_str() ) );

	// new	
	PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );
	assert(pRetval);

	retval = PyString_AsString( pRetval );

	DBGX2(DBG_CONFIG,"obj=`%s` attr=`%s` op=`%s`\n",
				name.c_str(),attrNameToString(attr).c_str(), retval.c_str() );

	Py_DECREF( pRetval );
	Py_DECREF( pFunc );
	Py_DECREF( pArgs );
	unlock();

	return retval;
}

std::deque< std::string > PyConfig::findChildren( std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());
	std::deque< std::string > children;

	lock();
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findChildren" );
	assert(pFunc);

	PyObject* pArgs = PyTuple_New( 1 );
	assert(pArgs);

	PyTuple_SetItem( pArgs, 0, PyString_FromString( name.c_str() ) );
	
	PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );
	assert(pRetval);

	for ( int i=0; i < PyList_Size( pRetval); i++ ) {
		char* str = PyString_AsString(PyList_GetItem(pRetval,i) );
		DBGX2(DBG_CONFIG,"%s \n", str );
		children.push_back( str ); 
	}

	Py_DECREF( pFunc );
	Py_DECREF( pArgs );
	Py_DECREF( pRetval );
	unlock();

	return children;
}

std::string PyConfig::findParent( std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());
	std::string retval;

	lock();
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findParent" );
	assert(pFunc);

	PyObject* pArgs = PyTuple_New( 1 );
	assert(pArgs);

	PyTuple_SetItem( pArgs, 0, PyString_FromString( name.c_str() ) );
	
	PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );

	if ( pRetval ) {
		retval = PyString_AsString(pRetval);
		Py_DECREF( pRetval );
	}
	DBGX2(DBG_CONFIG,"%s\n", retval.c_str() );

	Py_DECREF( pFunc );
	Py_DECREF( pArgs );
	unlock();
	return retval;
}

std::string PyConfig::findObjLocation( std::string name )
{
	DBGX2(DBG_CONFIG,"%s\n",name.c_str());
	std::string retval;

	lock();
	PyObject* pFunc = PyObject_GetAttrString( m_pModule, "findObjLocation" );
	assert(pFunc);

	PyObject* pArgs = PyTuple_New( 1 );
	assert(pArgs);

	PyTuple_SetItem( pArgs, 0,  PyString_FromString( name.c_str() ) );
	
	PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );

	if ( pRetval ) {
		retval = PyString_AsString(pRetval);
		Py_DECREF( pRetval );
	}
	DBGX2(DBG_CONFIG,"%s\n", retval.c_str() );

	Py_DECREF( pFunc );
	Py_DECREF( pArgs );
	unlock();
	return retval;
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
    default:               return "????";
    }
    return NULL;
}

PWR_ObjType PyConfig::objTypeStrToInt( const std::string name )
{
    if ( 0 == name.compare( "Platform" ) ) {
        return  PWR_OBJ_PLATFORM;
    } else if ( 0 == name.compare( "Cabinet" ) ) {
        return PWR_OBJ_CABINET;
    } else if ( 0 == name.compare( "Chassis" ) ) {
        return PWR_OBJ_CHASSIS;
    } else if ( 0 == name.compare( "Board" ) ) {
        return PWR_OBJ_BOARD;
    } else if ( 0 == name.compare( "Node" ) ) {
        return PWR_OBJ_NODE;
    } else if ( 0 == name.compare( "Socket" ) ) {
        return PWR_OBJ_SOCKET;
    } else if ( 0 == name.compare( "Core" ) ) {
        return PWR_OBJ_CORE;
    } else if ( 0 == name.compare( "Power Plane" ) ) {
        return PWR_OBJ_CORE;
    } else if ( 0 == name.compare( "Memory" ) ) {
        return PWR_OBJ_CORE;
    } else if ( 0 == name.compare( "Nic" ) ) {
        return PWR_OBJ_CORE;
    }
    return PWR_OBJ_INVALID;
}

std::string PyConfig::attrNameToString( PWR_AttrName name )
{
    switch( name ){
    case PWR_ATTR_PSTATE: return "PSTATE";
    case PWR_ATTR_CSTATE: return "CSTATE";
    case PWR_ATTR_CSTATE_LIMIT: return "CSTATE_LIMIT";
    case PWR_ATTR_SSTATE: return "SSTATE";
    case PWR_ATTR_CURRENT: return "CURRENT";
    case PWR_ATTR_VOLTAGE: return "VOLTAGE";
    case PWR_ATTR_POWER: return "POWER";
    case PWR_ATTR_POWER_LIMIT_MAX: return "MAX_POWER";
    case PWR_ATTR_POWER_LIMIT_MIN: return "MIN_POWER";
    case PWR_ATTR_FREQ: return "FREQ";
    case PWR_ATTR_FREQ_LIMIT_MIN: return "FREQ_MIN";
    case PWR_ATTR_FREQ_LIMIT_MAX: return "FREQ_MAX";
    case PWR_ATTR_ENERGY: return "ENERGY";
    case PWR_ATTR_TEMP: return "TEMP";
    case PWR_ATTR_OS_ID: return "OS_ID";
    case PWR_ATTR_THROTTLED_TIME: return "THROTTLED_TIME";
    case PWR_ATTR_THROTTLED_COUNT: return "THROTTLED_COUNT";
    default: return "";
    }
    return NULL;
}
