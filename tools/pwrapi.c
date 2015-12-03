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

#include "pwr.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define NUM_ATTR(X) (sizeof(X)/sizeof(PWR_AttrName))


int get_type_objects( PWR_Obj self, PWR_ObjType type, PWR_Grp outGroup )
{
    unsigned int i;
    size_t size;
    PWR_Grp cgrp;
    PWR_Obj obj;

    if( (cgrp=PWR_ObjGetChildren( self )) == PWR_NULL  ) {
        printf( "Error: getting child objects from PowerAPI context failed\n" );
        return -1;
    }

    size = PWR_GrpGetNumObjs( cgrp );
    for( i = 0; i < size; i++ ) {
        obj = PWR_GrpGetObjByIndx( cgrp, i );
        if( PWR_ObjGetType( obj ) == type ) {
            printf("add %s object\n",PWR_ObjGetName(obj));
            int rc = PWR_GrpAddObj( outGroup, obj );
            assert( rc!=PWR_RET_FAILURE );
        }
        int rc = get_type_objects( obj, type, outGroup );
        if ( rc != 0 ) return rc;
    }
    return 0;
}

int main( int argc, char* argv[] )
{
    PWR_Obj self;
    PWR_Cntxt cntxt;
    PWR_Grp grp;
    time_t time;
    double start = 0.0, val = 0.0;
    PWR_Time start_ts = 0, val_ts = 0;
    struct timeval t0, t1;
    unsigned long tdiff;

    int option;
    unsigned int i, j, samples = 1, freq = 1, numattrs = 0;
    PWR_ObjType type = PWR_OBJ_INVALID;

    PWR_AttrName attrs[100];
    PWR_Time vals_ts[NUM_ATTR(attrs)*1000];
    double vals[NUM_ATTR(attrs)*1000];
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
                        attrs[numattrs++] = PWR_ATTR_MAX_POWER;
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

    if( (cntxt=PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "Application" )) == 0x0 ) {
        printf( "Error: initialization of PowerAPI context failed\n" );
        return -1;
    }


    if( (self=PWR_CntxtGetEntryPoint( cntxt ) ) == 0x0 ) {
        printf( "Error: getting self from PowerAPI context failed\n" );
        return -1;
    }

    grp = PWR_GrpCreate( cntxt, "tmp" );
    assert( PWR_NULL != grp );

    if( type != PWR_OBJ_INVALID && 0 != get_type_objects( self, type, grp ) ) {
        printf( "Error: getting core objects failed\n" );
        return -1;
    }

    PWR_Status stats = PWR_StatusCreate();
    for( i = 0; i < samples; i++ ) {
        gettimeofday( &t0, 0x0 );

        if( PWR_GrpAttrGetValues( grp, numattrs, attrs, vals, vals_ts, stats ) 
                != PWR_RET_SUCCESS ) 
        {
            PWR_AttrAccessError error; 
            int rc = PWR_StatusPopError( stats, &error );  

            printf("Error: reading of `%s` failed for object `%s` with error %d\n",
                    PWR_AttrGetTypeString( error.name), 
                    PWR_ObjGetName(error.obj), error.error );

            return -1;
        }

        for( j = 0; j < numattrs; j++ ) {
            if( !i && attrs[j] == PWR_ATTR_ENERGY ) {
                start = vals[j];
                start_ts = vals_ts[j];
            }
            if( !j ) time = vals_ts[j] - start_ts;
            if( attrs[j] == PWR_ATTR_ENERGY )
                printf( "%lf ", vals[j] - start );
            printf( "%lf ", vals[j] );
        }
        printf( "%lf\n", time/1000000000.0 );

        gettimeofday( &t1, 0x0 );
        tdiff = t1.tv_sec - t0.tv_sec +
            (t1.tv_usec - t0.tv_usec) / 1000000.0;

        if( tdiff < 1000000.0 / freq )
            usleep( 1000000.0 / freq - tdiff );
    }

    return 0;
}
