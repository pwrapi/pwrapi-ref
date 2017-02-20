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

static void usage( const char* exename )
{
	fprintf(stderr,"%s: -o objectName [-s numberSamples] \n",exename);
}

int main( int argc, char* argv[] )
{
    PWR_Obj     obj;
    PWR_Cntxt   cntxt;
	int rc;
	int option;
	char* name = NULL;
	int samples = 1;

	while( (option=getopt( argc, argv, "o:s:" )) != -1 ) {
    	switch( option ) {
	      case 'o':
			name = optarg;
			break;
	      case 's':
			samples = atoi(optarg);
			break;
		  default:
			usage( argv[0] );
			exit(-1);
		}
	}

	if ( ! name ) {
		usage( argv[0] );
		exit(-1);
	}

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "App", &cntxt );
    assert( PWR_RET_SUCCESS == rc );

    rc = PWR_CntxtGetObjByName( cntxt, name, &obj );
    assert( PWR_RET_SUCCESS == rc );

    PWR_AttrName attr = PWR_ATTR_ENERGY;
    double value;
    PWR_Time time;

	double prev = 0;
	while ( samples-- ) {
		sleep(1);
    	rc = PWR_ObjAttrGetValue( obj, attr, &value, &time );
		if ( PWR_RET_SUCCESS != rc ) {
			printf("FATAL: could not read energy for object %s, rc=%d\n",name,rc);
			exit( -1);
		}
    	printf("%s: %.0lf joules, %.0lf watts\n",name, value, 
			prev == 0 ? 0 :	value-prev);
		prev = value;
	}

	PWR_CntxtDestroy( cntxt );

	return 0;
}	
