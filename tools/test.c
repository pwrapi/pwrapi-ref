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

int main( int argc, char* argv[] )
{
    PWR_Obj     obj;
    PWR_Cntxt   cntxt;
	int rc;

    assert( argc == 2 );

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "App", &cntxt );
    assert( PWR_RET_SUCCESS == rc );

    char* name = argv[1];

    rc = PWR_CntxtGetObjByName( cntxt, name, &obj );
    assert( PWR_RET_SUCCESS == rc );

    PWR_AttrName attr = PWR_ATTR_ENERGY;
    double value;
    PWR_Time time;
    rc = PWR_ObjAttrGetValue( obj, attr, &value, &time );
    assert( PWR_RET_SUCCESS == rc );
    printf("obj=%s attr=%s value=%lf\n",name,PWR_AttrGetTypeString(attr), value);

	PWR_CntxtDestroy( cntxt );

	return 0;
}	
