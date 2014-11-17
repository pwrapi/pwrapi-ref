#include "pow.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MICROSECONDS 1e6

static char usage[] =
    "usage: %s [-s samples] [-f freq]\n";

int main( int argc, char** argv )
{
    double power;
    PWR_Time timestamp;
    unsigned int option, sample, samples, freq;

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

    PWR_Cntxt cntxt = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "miniGhost" );
    PWR_Obj self = PWR_CntxtGetEntryPoint( cntxt );

    for( sample = 0; sample < samples; sample++ ) {
        PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, &power, &timestamp );
        if(sample) printf( "%lg %llu\n", power, timestamp );

        usleep( MICROSECONDS / freq );
    }

    return 0;
}
