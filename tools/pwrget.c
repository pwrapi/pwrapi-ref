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

static void usage( const char* exename )
{
	fprintf(stderr,"%s: -o objectName -a attrName\n",exename);
}

static PWR_AttrName getAttr( const char * );

static void toUpper( char* str )
{
	int i;
	for ( i = 0; i < strlen(str); i++ ) {
		str[i] = toupper(str[i]);
	} 
}

int main( int argc, char* argv[] )
{
    PWR_Obj     obj;
    PWR_Cntxt   cntxt;
	int rc;
	int option;
	char* name = NULL;
	char* attrName = NULL;

	while( (option=getopt( argc, argv, "o:a:" )) != -1 ) {
    	switch( option ) {
	      case 'o':
			name = optarg;
			break;

	      case 'a':
			attrName = optarg;
			break;
		  default:
			usage( argv[0] );
			exit(-1);
		}
	}

	if ( ! name || ! attrName ) {
		usage( argv[0] );
		exit(-1);
	}
	toUpper( attrName );
	
    PWR_AttrName attr = getAttr( attrName );
	assert( attr != PWR_ATTR_INVALID );

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "App", &cntxt );
    assert( PWR_RET_SUCCESS == rc );

    rc = PWR_CntxtGetObjByName( cntxt, name, &obj );
    assert( PWR_RET_SUCCESS == rc );

    double value;
    PWR_Time time;

  	rc = PWR_ObjAttrGetValue( obj, attr, &value, &time );
    if( PWR_RET_SUCCESS != rc ) {
   		fprintf(stderr,"Get Failed: %s: %s\n",name, attrName);
		return -1;
	}
   	printf("%s: %s %.0lf\n",name, attrName, value);

	PWR_CntxtDestroy( cntxt );

	return 0;
}	

PWR_AttrName getAttr( const char * name )
{

	if ( ! strcmp( name, "PSTATE" ) ) {
		return PWR_ATTR_PSTATE;
	} else if ( ! strcmp( name, "CSTATE" ) ) {
		return PWR_ATTR_CSTATE;
	} else if ( ! strcmp( name, "CSTATE" ) ) {
		return PWR_ATTR_CSTATE;
	} else if ( ! strcmp( name, "SSTATE" ) ) {
		return PWR_ATTR_SSTATE;
	} else if ( ! strcmp( name, "CURRENT" ) ) {
		return PWR_ATTR_CURRENT;
	} else if ( ! strcmp( name, "VOLTAGE" ) ) {
		return PWR_ATTR_VOLTAGE;
	} else if ( ! strcmp( name, "POWER" ) ) {
		return PWR_ATTR_POWER;
	} else if ( ! strcmp( name, "FREQ" ) ) {
		return PWR_ATTR_FREQ;
	} else if ( ! strcmp( name, "ENERGY" ) ) {
		return PWR_ATTR_ENERGY;
	} else if ( ! strcmp( name, "TEMP" ) ) {
		return PWR_ATTR_TEMP;
	} else if ( ! strcmp( name, "OS_ID" ) ) {
		return PWR_ATTR_OS_ID;
	}
	return PWR_ATTR_INVALID;
}
