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

#include "object.h"
#include "request.h"
#include "status.h"
#include "debug.h"
#include "cntxt.h"
#include "attrInfo.h"
#include "device.h"
#include "util.h"
#include "communicator.h"

using namespace PowerAPI;

Object::Object( std::string name, PWR_ObjType type, Cntxt* ctx ) :
	m_name(name), m_objType(type), m_cntxt(ctx),
    m_parent( NULL ), m_children( NULL ), m_attrInfo( PWR_NUM_ATTR_NAMES )
{
	DBGX("%s %s\n",name.c_str(), objTypeToString(type) );

	for ( int attr = PWR_ATTR_PSTATE; attr < PWR_NUM_ATTR_NAMES; attr++ ) { 
		m_attrInfo[ attr ] = m_cntxt->initAttr( this, (PWR_AttrName)attr );
	}
}

Object::~Object()
{
	for ( int attr = PWR_ATTR_PSTATE; attr < PWR_NUM_ATTR_NAMES; attr++ ) { 
		delete m_attrInfo[attr];
	} 
}


Object* Object::parent()
{	
	DBGX("\n");
	if ( ! m_parent ) {
		m_parent = m_cntxt->getParent( this );
	}
	return m_parent;
}

Grp* Object::children()
{
	DBGX("\n");
	if ( ! m_children ) {
		m_children = m_cntxt->getChildren( this );
	}
	return m_children;
}

bool Object::attrIsValid( PWR_AttrName attr )
{
	DBGX("\n");
	return m_attrInfo[attr]->isValid();
}

int Object::attrGetValues( int count, PWR_AttrName names[], void* buf,
	                           PWR_Time ts[], Status* status )
{
	uint64_t* ptr = (uint64_t*) buf; 

	DBGX("\n");

	for ( int i = 0; i < count; i++ ) {
		ptr[i] = 0;
		ts[i] = 0;

		if ( ! m_attrInfo[ names[i] ]->isValid() ) {
			status->add( this, names[i], PWR_RET_INVALID );
			break;
		}

		int retval = attrGetValuesDevice( *m_attrInfo[ names[i] ], 
							names[i], &ptr[i], &ts[i] );
		if ( PWR_RET_SUCCESS != retval ) {
			status->add( this, names[i], retval );
			break;
		}
	}

	return status->empty() ? PWR_RET_SUCCESS : PWR_RET_STATUS;
}

int Object::attrGetValuesDevice( AttrInfo& info, PWR_AttrName attr,
		            void* buf, PWR_Time* timeStamp )
{
	DBGX("%s %s\n",name().c_str(),attrNameToString(attr));
		
	std::vector<uint64_t> value(info.devices.size());
	std::vector<PWR_Time> tmpTS(info.devices.size());

	for ( unsigned i = 0 ; i < info.devices.size(); i++ ) {
		int retval = info.devices[i]->getValue( attr, &value[i], 8, &tmpTS[i] );
		if ( PWR_RET_SUCCESS != retval ) {
			return retval;
		}
	}

	if ( info.devices.size() ) {
		info.operation( buf, &value[0], value.size() );
		*timeStamp = info.calcTime( tmpTS );
	}
	return PWR_RET_SUCCESS;
}

int Object::attrSetValues( int count, PWR_AttrName names[], void* buf,
                            Status* status  )
{
	uint64_t* ptr = (uint64_t*) buf; 

	for ( int i = 0; i < count; i++ ) {

	DBGX("%s %s\n",name().c_str(),attrNameToString(names[i]));
		if ( ! m_attrInfo[ names[i] ]->isValid() ) {
			DBGX("invalid %s %s\n",name().c_str(),attrNameToString(names[i]));
			status->add( this, names[i], PWR_RET_INVALID );
			break;
		}


		int retval = attrSetValuesDevice( *m_attrInfo[ names[i] ], 
							names[i], &ptr[i] );
		if ( PWR_RET_SUCCESS != retval ) {
			status->add( this, names[i], retval );
			break;
		}
	}

	return status->empty() ? PWR_RET_SUCCESS : PWR_RET_STATUS;
}

int Object::attrSetValuesDevice( AttrInfo& info, PWR_AttrName attr,
													void* buf )
{
	DBGX("%s %s\n",name().c_str(),attrNameToString(attr));

	for ( unsigned i = 0 ; i < info.devices.size(); i++ ) {
		int retval = info.devices[i]->setValue( attr, buf, 8 );
		if ( PWR_RET_SUCCESS != retval ) {
			return retval;
		}
	}
	return PWR_RET_SUCCESS;
}

int Object::attrGetValue( PWR_AttrName attr, void* buf, PWR_Time* ts ) {
    Status status;
    int retval;
	DBGX("%s %s\n",name().c_str(),attrNameToString(attr));
    retval = Object::attrGetValues( 1, &attr, buf, ts, &status );
    if ( retval != PWR_RET_SUCCESS ) {
        PWR_AttrAccessError error;
        retval = status.pop( &error );
        assert( retval == PWR_RET_SUCCESS );
        return error.error;
    }
    return PWR_RET_SUCCESS;
}


int Object::attrSetValue( PWR_AttrName attr, void* buf ) {
    Status status;
    int retval;
	DBGX("%s %s\n",name().c_str(),attrNameToString(attr));
    retval = Object::attrSetValues( 1, &attr, buf, &status );
    if ( retval != PWR_RET_SUCCESS ) {
        PWR_AttrAccessError error;
        retval = status.pop( &error );
        assert( retval == PWR_RET_SUCCESS );
        return error.error;
    }
    return PWR_RET_SUCCESS;
}

//================================================================
int Object::attrStartLog( PWR_AttrName name )
{
	DBGX("\n");

	if ( ! m_attrInfo[ name ]->isValid() ) {
		return PWR_RET_FAILURE; 
	}

	AttrInfo& info = *m_attrInfo[name];

	for ( unsigned i = 0 ; i < info.devices.size(); i++ ) {
		int retval = info.devices[i]->startLog( name );
		if ( PWR_RET_SUCCESS != retval ) {
			return retval;
		}
	}

	return PWR_RET_SUCCESS;
}

int Object::attrStopLog( PWR_AttrName name )
{
	DBGX("\n");

	if ( ! m_attrInfo[ name ]->isValid() ) {
		return PWR_RET_FAILURE; 
	}

	AttrInfo& info = *m_attrInfo[name];

	for ( unsigned i = 0; i < info.devices.size(); i++ ) {
		int retval = info.devices[i]->stopLog( name );
		if ( PWR_RET_SUCCESS != retval ) {
			return retval;
		}
	}
	return PWR_RET_SUCCESS;
}

int Object::attrGetSamples( PWR_AttrName name, PWR_Time* start, 
		double period, unsigned int* count, void* buf )
{
	DBGX("\n");

	if ( ! m_attrInfo[ name ]->isValid() ) {
		return PWR_RET_FAILURE; 
	}

	AttrInfo& info = *m_attrInfo[name];

	assert( info.devices.size() <= 1 );
	for ( unsigned i = 0; i < info.devices.size(); i++ ) {
		std::vector<uint64_t> data(*count);

		unsigned int tCnt = *count; 
		PWR_Time tStart;

		int retval = info.devices[i]->getSamples( name, 
				&tStart, period, &tCnt, &data[0] );

		if ( PWR_RET_SUCCESS != retval ) {
			return retval;
		}
		for ( unsigned i = 0; i < data.size(); i++ ) {
			// WE NEED to some sort of op here 
			// WE NEED to compare start times for each device and message the results
			((uint64_t*)buf)[i] += data[i];
		}
		*start = tStart;
	}
	return PWR_RET_SUCCESS;
}
