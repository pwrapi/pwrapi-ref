#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "pow.h"

int main( int argc, char* argv[] )
{
    PWR_Obj   self;
    PWR_Cntxt cntxt;
    time_t    time;
    double    energy_start = 0.0, energy_end = 0.0;
    PWR_Time  energy_start_ts = 0, energy_end_ts = 0;

    if( (cntxt=PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "MiniMD" )) == 0x0 ) {
      printf( "Error: initialization of PowerAPI context failed\n" );
      return -1;
    }

    if( (self=PWR_CntxtGetSelf( cntxt )) == 0x0 ) {
      printf( "Error: getting self from PowerAPI context failed\n" );
      return -1;
    }
    printf("Profiling `%s`\n", PWR_ObjGetTypeString( PWR_ObjGetType( self ) ) ); 

    if( PWR_ObjAttrGetValue( self, PWR_ATTR_ENERGY, &energy_start, &energy_start_ts ) == PWR_ERR_INVALID ) {
      printf( "Error: getting attribute value from PowerAPI self failed\n" );
      return -1;
    }
    PWR_TimeConvert( energy_start_ts, &time );
    printf( "Starting Energy at time %s is %lf\n", ctime(&time), energy_start );

    sleep( 10 );

    if( PWR_ObjAttrGetValue( self, PWR_ATTR_ENERGY, &energy_end, &energy_end_ts ) == PWR_ERR_INVALID ) {
      return -1;
    }
    PWR_TimeConvert( energy_end_ts, &time );
    printf( "Ending Energy at time %s is %lf\n", ctime(&time), energy_end );

    printf( "Total Energy over %f seconds is %lf\n",
            (unsigned long long)(energy_end_ts - energy_start_ts)/1000000000.0, energy_end - energy_start );

    return 0;
}
