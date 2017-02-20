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

#include "pwr.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define NUM_ATTR(X) (sizeof(X)/sizeof(PWR_AttrName))


void get_type_objects( PWR_Obj obj, PWR_ObjType type, PWR_Grp outGroup )
{
    unsigned int i;
    size_t size;
    PWR_Grp cgrp;
	PWR_ObjType objType;
	PWR_ObjGetType( obj, &objType );

    if( objType == type ) {
        if( PWR_GrpAddObj( outGroup, obj ) == PWR_RET_FAILURE ) {
            printf( "Error: failed\n" );
            return;
        }
    }

	int rc = PWR_ObjGetChildren( obj, &cgrp );
    if ( PWR_RET_SUCCESS != rc ) {  
        printf( "Error: failed\n" );
		return;
    }
	if ( NULL == cgrp ) {
		return;
	}

    size = PWR_GrpGetNumObjs( cgrp );
    for( i = 0; i < size; i++ ) {
		PWR_Obj tmp;
		PWR_GrpGetObjByIndx( cgrp, i, &tmp), 
        get_type_objects( tmp, type, outGroup );
    }
}

int main( int argc, char* argv[] )
{
	int rc;
    PWR_Obj self;
    PWR_Cntxt cntxt;
    PWR_Grp grp;
    unsigned long time;
    double start = 0.0, val = 0.0;
    PWR_Time start_ts = 0, val_ts = 0;
    struct timeval t0, t1;
    unsigned long tdiff;

    int option;
    unsigned int i, j, k, samples = 1, freq = 1, numobjs = 0, numattrs = 0;
    PWR_ObjType type = PWR_OBJ_SOCKET;

    PWR_AttrAccessError error; 
    PWR_AttrName attrs[100];
    PWR_Time *vals_ts = 0x0;
    double *vals = 0x0;
    static char usage[] =
        "usage: %s [-s samples] [-f freq] [-a attr] [-h]\n";

    while( (option=getopt( argc, argv, "s:f:a:t:h" )) != -1 )
        switch( option ) {
            case 's':
                samples = atoi(optarg);
                break;
            case 'f':
                freq = atoi(optarg);
                break;
            case 'a':
                switch( optarg[0] ) {
                    case 'E':
                        attrs[numattrs++] = PWR_ATTR_ENERGY;
                        break;
                    case 'P':
                        attrs[numattrs++] = PWR_ATTR_POWER;
                        break;
                    case 'F':
                        attrs[numattrs++] = PWR_ATTR_FREQ;
                        break;
                    case 'T':
                        attrs[numattrs++] = PWR_ATTR_TEMP;
                        break;
                    case 'M':
                        attrs[numattrs++] = PWR_ATTR_POWER_LIMIT_MAX;
                        break;
                    case 'V':
                        attrs[numattrs++] = PWR_ATTR_VOLTAGE;
                        break;
                    case 'C':
                        attrs[numattrs++] = PWR_ATTR_CURRENT;
                        break;
                    default:
                        printf( "Error: unsupported attribute type (try E P F T M V C)\n" );
                        return -1;
                } 
                break;
            case 't':
                switch( optarg[0] ) {
                    case 'P':
                        type = PWR_OBJ_PLATFORM;
                        break;
                    case 'A':
                        type = PWR_OBJ_CABINET;
                        break;
                    case 'B':
                        type = PWR_OBJ_BOARD;
                        break;
                    case 'N':
                        type = PWR_OBJ_NODE;
                        break;
                    case 'S':
                        type = PWR_OBJ_SOCKET;
                        break;
                    case 'C':
                        type = PWR_OBJ_CORE;
                        break;
                    case 'M':
                        type = PWR_OBJ_MEM;
                        break;
                    case 'I':
                        type = PWR_OBJ_NIC;
                        break;
                    default:
                        printf( "Error: unsupported object type (try P B N S C M I)\n" );
                        return -1;
                }
                break;
            case 'h':
            case '?':
                fprintf( stderr, usage, argv[0] );
                exit( 1 );
        }

    if ( PWR_OBJ_INVALID == type || 0 == numattrs ) {
        fprintf( stderr, usage, argv[0] );
        exit( 1 );
    } 

	rc = PWR_CntxtInit(PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "Application", &cntxt);
    if( PWR_RET_SUCCESS != rc ) {
        printf( "Error: initialization of PowerAPI context failed\n" );
        return -1;
    }

	rc = PWR_CntxtGetEntryPoint( cntxt, &self ) ;
    if( PWR_RET_SUCCESS != rc ) {
        printf( "Error: getting self from PowerAPI context failed\n" );
        return -1;
    }

	rc = PWR_GrpCreate( cntxt, &grp );
    if( PWR_RET_SUCCESS != rc ) {
        printf( "Error: creating a group from PowerAPI failed\n" );
        return -1;
    }

    get_type_objects( self, type, grp );
    if( !(numobjs=PWR_GrpGetNumObjs( grp )) ) {
        printf( "Error: getting type objects failed\n" );
        return -1;
    }
    vals_ts = malloc( numobjs * numattrs * sizeof(PWR_Time) );
    vals = malloc( numobjs * numattrs * sizeof(double) );

    PWR_Status stats;
	PWR_StatusCreate( cntxt, &stats );
    for( i = 0; i < samples; i++ ) {
        gettimeofday( &t0, 0x0 );

        if( PWR_GrpAttrGetValues( grp, numattrs, attrs, vals, vals_ts, stats ) 
                == PWR_RET_SUCCESS ) {
            for( j = 0; j < numobjs; j++ ) {
				char name[100]; 
				PWR_Obj obj;
				PWR_GrpGetObjByIndx( grp, j, &obj ), 
				PWR_ObjGetName( obj, name, 100 );
                printf( "%s: ", name );

                for( k = 0; k < numattrs; k++ ) {
                    if( !k ) {
                        if( !i ) {
                            start = vals[k];
                            start_ts = vals_ts[k];
                        }
                        time = vals_ts[k] - start_ts;
                    }
                    if( attrs[k] == PWR_ATTR_ENERGY )
                        printf( "%lf ", vals[k] - start );
                    printf( "%lf ", vals[k] );
                }
                printf( "%lf\n", time/1000000000.0 );

                gettimeofday( &t1, 0x0 );
                tdiff = t1.tv_sec - t0.tv_sec +
                    (t1.tv_usec - t0.tv_usec) / 1000000.0;

                if( tdiff < 1000000.0 / freq )
                    usleep( 1000000.0 / freq - tdiff );
            }
        } else {
            PWR_StatusPopError( stats, &error );  
			char name[100];
            PWR_ObjGetName(error.obj, name, 100 );
            printf("Error: reading of `%s` failed for object `%s` with error %d\n",
                    PWR_AttrGetTypeString( error.name), name, error.error );
            break;
        }
    }

    free( vals_ts );
    free( vals );

    return 0;
}
