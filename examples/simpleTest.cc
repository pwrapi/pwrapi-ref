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

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <string>

#include <pwr.h> 

char* myctime(const time_t *timep);

double getTime() {
	struct timespec spec;
	int rc = clock_gettime( CLOCK_REALTIME, &spec );
	assert( 0 == rc );

    return (spec.tv_sec * 1000) + ((double) spec.tv_nsec / 1000000.0);
}

int main( int argc, char* argv[] )
{
    PWR_Grp     grp;
    PWR_Obj     self;
    PWR_Cntxt   cntxt;
    time_t      time;
    int         retval;
    double       value;
    PWR_Time ts;
	PWR_Status  status;
	int count = 1;
	int forever = 0;

	assert( argc == 3 || argc == 2 );	

	if ( 3 == argc ) {
 		count = atoi(argv[2]);
	}

	if ( 0 == count ) {
		forever = 1;	
	}

	char* xxx = getenv("WAIT");
	if ( xxx ) {
		int sec = atoi(xxx);
		printf("sleep %d\n",sec);
		sleep(sec);
		printf("lets go\n");
	}

    // Get a context
    cntxt = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "App" );
    assert( PWR_NULL != cntxt   );

	PWR_Obj obj = PWR_CntxtGetObjByName( cntxt, argv[1] );
    assert( PWR_NULL != obj   );

	std::string arg0 = argv[0];
	std::string exe = arg0.substr(arg0.find_last_of("/")+1); 
	while ( forever || count-- ) {	
			
		PWR_AttrName names[] = { PWR_ATTR_ENERGY} ;
		value = 0;
		struct timeval tvStart,tvEnd;
		double start = getTime();
    	retval = PWR_ObjAttrGetValue( obj, PWR_ATTR_ENERGY, &value, &ts );
		double stop = getTime();
    	assert( retval == PWR_RET_SUCCESS );

		printf("%s: \'%s\' value=%.1lf, latency %f\n",exe.c_str(), argv[1], value/1000000, stop - start);
		sleep(1);
	}
	PWR_CntxtDestroy( cntxt );
    return 0;
}
char* myctime(const time_t *timep)
{
    char* tmp = ctime(timep);
    tmp[ strlen(tmp) - 1 ] = 0; 
    return tmp;
}
