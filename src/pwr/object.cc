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
    m_parent( NULL ), m_children( NULL )
{
	DBGX("%s %s\n",name.c_str(), objTypeToString(type) );
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
	initAttrIfNeeded( attr );
	return m_attrInfo[attr]->isValid();
}

int Object::attrGetValues( int count, PWR_AttrName names[], void* buf,
	                           PWR_Time ts[], Status* status )
{
	uint64_t* ptr = (uint64_t*) buf; 

	DBGX("\n");

	for ( int i = 0; i < count; i++ ) {
		initAttrIfNeeded( names[i] );
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

	return status->empty() ? PWR_RET_SUCCESS : PWR_RET_FAILURE;
}

int Object::attrGetValuesDevice( AttrInfo& info, PWR_AttrName name,
		            void* buf, PWR_Time* timeStamp )
{
	DBGX("\n");
		
	std::vector<uint64_t> value(info.devices.size());
	std::vector<PWR_Time> tmpTS(info.devices.size());

	for ( unsigned i = 0 ; i < info.devices.size(); i++ ) {
		int retval = info.devices[i]->getValue( name, &value[i], 8, &tmpTS[i] );
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

	DBGX("\n");
	for ( int i = 0; i < count; i++ ) {
		initAttrIfNeeded( names[i] );

		if ( ! m_attrInfo[ names[i] ]->isValid() ) {
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

	return status->empty() ? PWR_RET_SUCCESS : PWR_RET_FAILURE;
}

int Object::attrSetValuesDevice( AttrInfo& info, PWR_AttrName name,
													void* buf )
{
	DBGX("\n");

	for ( unsigned i = 0 ; i < info.devices.size(); i++ ) {
		int retval = info.devices[i]->setValue( name, buf, 8 );
		if ( PWR_RET_SUCCESS != retval ) {
			return retval;
		}
	}
	return PWR_RET_SUCCESS;
}

int Object::attrGetValue( PWR_AttrName attr, void* buf, PWR_Time* ts ) {
    Status status;
    int retval;
	DBGX("\n");
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
	DBGX("\n");
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
	initAttrIfNeeded( name );

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
	initAttrIfNeeded( name );

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

int Object::attrGetSamples( PWR_AttrName name, PWR_Time* ts, 
		double period, unsigned int* count, void* buf )
{
	DBGX("\n");
	initAttrIfNeeded( name );

	if ( ! m_attrInfo[ name ]->isValid() ) {
		return PWR_RET_FAILURE; 
	}

	AttrInfo& info = *m_attrInfo[name];

	for ( unsigned i = 0; i < info.devices.size(); i++ ) {
		std::vector<uint64_t> data(*count);
		unsigned int tCnt = *count; 
		PWR_Time tStart;
		int retval = info.devices[i]->getSamples( name, &tStart, period, &tCnt, &data[0] );
		if ( PWR_RET_SUCCESS != retval ) {
			return retval;
		}
		for ( unsigned i = 0; i < data.size(); i++ ) {
			// WE NEED to some sort of op here 
			// WE NEED to compare start times for each device and message the results
			((uint64_t*)buf)[i] += data[i];
		}
		*ts = tStart;
	}
	return PWR_RET_SUCCESS;
}
