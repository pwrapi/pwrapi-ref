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

#include <pwr.h> 

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

void walk( PWR_Obj node, int level )
{
	int rc;
    char name[100];
	PWR_ObjType objType;

    PWR_ObjGetName( node, name, 100 );
	PWR_ObjGetType( node, &objType );

	char* indent = malloc( level*4 + 1);
	int i;
	for ( i = 0; i < level*4; i++) {
		indent[i] = ' ';
	}
	indent[level*4 ] = 0;

    printf("%sname=`%s` type=%s: ", indent, name,PWR_ObjGetTypeString( objType ) ); 
	for ( i = 0; i < PWR_NUM_ATTR_NAMES; i++) {
		if ( PWR_ObjAttrIsValid( node, i ) ) {
			printf("%s ",PWR_AttrGetTypeString(i));
		}
	}
	printf("\n");

    PWR_Grp children;
	rc = PWR_ObjGetChildren( node, &children );
	assert( rc >= PWR_RET_SUCCESS );	

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
