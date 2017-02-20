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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>


#if USE_CRAY_POWER_API
#include <cray-powerapi/api.h>
#else
#include <pwr.h>
#endif

static void walk( PWR_Obj node, int level );

int main( int argc, char* argv[] )
{
    PWR_Obj     self;
    PWR_Cntxt   cntxt;
	int rc;

    // Get a context
    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "App", &cntxt );
    assert( PWR_RET_SUCCESS == rc );

    rc = PWR_CntxtGetEntryPoint( cntxt, &self );
    assert( PWR_RET_SUCCESS == rc );
    
	walk( self, 0 );

	PWR_CntxtDestroy( cntxt );

	return 0;
}	
#if USE_CRAY_POWER_API
static inline const char* objTypeToString( PWR_ObjType type )
{
    switch( type ) {
    case PWR_OBJ_PLATFORM:    return "Platform";
    case PWR_OBJ_CABINET:     return "Cabinet";
    case PWR_OBJ_CHASSIS:     return "Chassis";
    case PWR_OBJ_BOARD:       return "Board";
    case PWR_OBJ_NODE:        return "Node";
    case PWR_OBJ_SOCKET:      return "Socket";
    case PWR_OBJ_CORE:        return "Core";
    case PWR_OBJ_POWER_PLANE: return "PowerPlane";
    case PWR_OBJ_MEM:         return "Memory";
    case PWR_OBJ_NIC:         return "Nic";
    case PWR_OBJ_HT:          return "HardwareThread";
    case PWR_OBJ_INVALID:     return "Invalid";
    default: return "????";
    }
    return NULL;
}
const char* PWR_ObjGetTypeString( PWR_ObjType type )
{
	return objTypeToString( type );
}
#endif

void walk( PWR_Obj node, int level )
{
	int rc;
    char name[100];
	PWR_ObjType objType;

    PWR_ObjGetName( node, name, 100 );
	PWR_ObjGetType( node, &objType );

	char* indent = (char*)malloc( level*4 + 1);
	int i;
	for ( i = 0; i < level*4; i++) {
		indent[i] = ' ';
	}
	indent[level*4 ] = 0;

    printf("%sname=`%s` type=%s: ", indent, name,PWR_ObjGetTypeString( objType ) ); 
	for ( i = 0; i < PWR_NUM_ATTR_NAMES; i++) {
		if ( PWR_ObjAttrIsValid( node, (PWR_AttrName)i ) ) {
#if USE_CRAY_POWER_API
			char buf[100];
			CRAYPWR_AttrGetName( (PWR_AttrName) i, buf,100);
			printf("%s ",buf);
#else
			printf("%s ",PWR_AttrGetTypeString(i));
#endif
		}
	}
	printf("\n");

    PWR_Grp children;
	rc = PWR_ObjGetChildren( node, &children );
	if ( rc == PWR_RET_WARN_NO_CHILDREN ) {
		return;
	}
	assert( rc == PWR_RET_SUCCESS );	

    for ( i = 0; i < PWR_GrpGetNumObjs(children); i++ ) {
		char name[100];
		PWR_Obj obj;
        PWR_GrpGetObjByIndx( children, i, &obj );
		walk( obj, level + 1 );
    }

#if 0
    PWR_Obj parent;
	rc = PWR_ObjGetParent( node , &parent );
	assert( rc >= PWR_RET_SUCCESS );	
    PWR_ObjGetName( parent, name, 100 );
    printf("%sparent=`%s`\n", indent, name ); 
#endif
}
