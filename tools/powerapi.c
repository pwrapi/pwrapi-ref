/*
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
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
#include <string.h>
#include <unistd.h>

#include "pow.h"

int main( int argc, char* argv[] )
{
    volatile unsigned long test = 0;

    PWR_Obj self;
    PWR_Cntxt cntxt;
    time_t time;
    double start = 0.0, val = 0.0;
    PWR_Time start_ts = 0, val_ts = 0;
    PWR_AttrName attr = PWR_ATTR_ENERGY;

    unsigned int i, j, option, verbose = 0, samples = 1, freq = 1, busy = 0;
    static char usage[] =
        "usage: %s [-s samples] [-f freq] [-a attr] [-v] [-h]\n";

    while( (option=getopt( argc, argv, "s:f:a:bvh" )) != -1 )
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
                        attr = PWR_ATTR_ENERGY;
                        break;
                    case 'P':
                        attr = PWR_ATTR_POWER;
                        break;
                    case 'F':
                        attr = PWR_ATTR_FREQ;
                        break;
                    case 'T':
                        attr = PWR_ATTR_TEMP;
                        break;
                    case 'M':
                        attr = PWR_ATTR_MAX_POWER;
                        break;
                    default:
                        printf( "Error: unsupported attribute type (try E P F T or M)\n" );
                        return -1;
                } 
                break;
            case 'b':
                busy = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
            case '?':
                fprintf( stderr, usage, argv[0] );
                exit( 1 );
        }

    if( verbose )
        printf( "samples=%d, freq=%d, attr=%d\n", samples, freq, attr );

    if( (cntxt=PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "MiniMD" )) == 0x0 ) {
        printf( "Error: initialization of PowerAPI context failed\n" );
        return -1;
    }

    if( (self=PWR_CntxtGetEntryPoint( cntxt )) == 0x0 ) {
        printf( "Error: getting self from PowerAPI context failed\n" );
        return -1;
    }
    if( verbose )
        printf( "Profiling `%s`\n", PWR_ObjGetTypeString( PWR_ObjGetType( self ) ) ); 

    for( i = 0; i < samples; i++ ) {
        if( PWR_ObjAttrGetValue( self, attr, &val, &val_ts ) == PWR_RET_INVALID ) {
            printf( "Error: reading of PowerAPI attribute failed\n" );
            return -1;
        }

        if( !i && attr == PWR_ATTR_ENERGY ) {
            start = val;
            start_ts = val_ts;
        }

        PWR_TimeConvert( val_ts, &time );
        printf( "Value is %lf at time %s", val, ctime(&time) );

	if( busy )
		for( j = 0; j < 40000000; j++ ) test = 2*j;
        else
		usleep( 1000000.0 / freq);
    }

    if( attr == PWR_ATTR_ENERGY )
        printf( "Total energy is %lf Joules over %f seconds\n", val - start,
                (unsigned long long)(val_ts - start_ts)/1000000000.0 );

    return 0;
}
