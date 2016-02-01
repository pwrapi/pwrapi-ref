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
#include <unistd.h>

#define MICROSECONDS 1e6

static char usage[] =
    "usage: %s [-s samples] [-f freq]\n";

int main( int argc, char** argv )
{
    double power = 0.0;
    PWR_Time timestamp = 0;
    unsigned int option, sample, samples = 1, freq = 1;

    while( (option=getopt( argc, argv, "s:f:h" )) != -1 )
        switch( option ) {
            case 's':
                samples = atoi(optarg);
                break;
            case 'f':
                freq = atoi(optarg);
                break;
            default:
                fprintf( stderr, usage, argv[0] );
                return -1;
        }

    PWR_Cntxt cntxt = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "App" );
    if( cntxt == 0x0 ) {
        printf( "ABORT - error occured when initializing context\n" );
        exit( 0 );
    }

    PWR_Obj self = PWR_CntxtGetEntryPoint( cntxt );
    if( self == 0x0 ) {
        printf( "ABORT - error occured when getting entry point\n" );
        exit( 0 );
    }

    for( sample = 0; sample < samples; sample++ ) {
        if( PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, &power, &timestamp ) != 0 ) {
            printf( "ABORT - error occured when querying power\n" );
            exit( 0 );
        }
        if(sample) printf( "%lg %llu\n", power, (unsigned long long)timestamp );

        usleep( MICROSECONDS / freq );
    }

    return 0;
}
