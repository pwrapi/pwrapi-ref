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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define NUM_ATTR(X) (sizeof(X)/sizeof(PWR_AttrName))

int main( int argc, char* argv[] )
{
    PWR_Obj self, obj;
    PWR_Grp grp;
    PWR_Cntxt cntxt;
    size_t size;
    unsigned long long time;
    double start = 0.0;
    PWR_Time start_ts = 0;
    struct timeval t0, t1;
    unsigned long tdiff;

    PWR_AttrName attrs[] = { PWR_ATTR_CURRENT, PWR_ATTR_POWER, PWR_ATTR_ENERGY };
    PWR_Time vals_ts[NUM_ATTR(attrs)*1000];
    double vals[NUM_ATTR(attrs)*1000];
    int stats[NUM_ATTR(attrs)];

    int option;
    char *token, *range, name[128] = "";
    unsigned int i, j, g0, g1, samples = 1, freq = 1, count = 0, g[1000];
    static char usage[] =
        "usage: %s [-s samples] [-f freq] [-r range] [-h]\n";

    while( (option=getopt( argc, argv, "s:f:r:h" )) != -1 )
        switch( option ) {
            case 's':
                samples = atoi(optarg);
                break;
            case 'f':
                freq = atoi(optarg);
                break;
            case 'r':
                while( (range=strtok( count ? optarg : NULL, "," )) ) {
                    g0 = g1 = atoi(range);
                    if( (range=strtok( range, "-" )) )
                        g1 = atoi(range);
                    for( i = g0; i < g1; i++ )
                        g[count++] = i;
                    printf( "ADDING GROUP %u to %u\n", g0, g1 );
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

    if( (self=PWR_CntxtGetEntryPoint( cntxt )) == 0x0 ) {
        printf( "Error: getting self from PowerAPI context failed\n" );
        return -1;
    }
/*
    if( (obj=PWR_CntxtGetObjByName( cntxt, "teller.node40" )) == 0x0 ) {
        printf( "Error: getting object by name from PowerAPI context failed\n" );
        return -1;
    }
*/
    if( (grp=PWR_ObjGetChildren( self )) == 0x0 ) {
        printf( "Error: getting object by name from PowerAPI context failed\n" );
        return -1;
    }
    size = PWR_GrpGetNumObjs( grp );
    for( i = 0; i < size; i++ ) {
        obj = PWR_GrpGetObjByIndx( grp, i );
        for( j = 0; j < count; j++ ) {
            sprintf( name, "shepard.node%u", g[j] );
            if( !(strcmp( PWR_ObjGetName( obj ), name )) )
                PWR_GrpAddObj( grp, obj );
        }
    }

    for( i = 0; i < samples; i++ ) {
        gettimeofday( &t0, 0x0 );

        if( PWR_GrpAttrGetValues( grp, NUM_ATTR(attrs), attrs, vals, vals_ts, stats ) == PWR_RET_INVALID ) {
            printf( "Error: reading of PowerAPI attributes failed\n" );
            return -1;
        }
        for( j = 0; j < NUM_ATTR(attrs); j++ ) {
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
