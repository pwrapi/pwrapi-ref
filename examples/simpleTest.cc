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

#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <getopt.h>

#include <pwr.h> 

char* myctime(const time_t *timep);

#ifdef __MACH__
#include <sys/time.h>
#define CLOCK_REALTIME 0
//clock_gettime is not implemented on OSX
int clock_gettime(int /*clk_id*/, struct timespec* t) {
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
#endif

double getTime() {
	struct timespec spec;
	int rc = clock_gettime( CLOCK_REALTIME, &spec );
	assert( 0 == rc );

    return (spec.tv_sec * 1000) + ((double) spec.tv_nsec / 1000000.0);
}


static FILE* _logFP = stdout; 	
int main( int argc, char* argv[] )
{
    PWR_Obj     self;
    PWR_Cntxt   cntxt;
    int         retval;
    double      value;
    PWR_Time 	ts;
	int count = 1;
	int forever = 0;
	int delay = 0;

	std::string logfile;	
	std::string object;

    int opt = 0;
    int long_index = 0;
    enum { LOG_FILE, OBJECT, COUNT, DELAY };
    static struct option long_options[] = {
        {"delay"          , optional_argument, NULL, DELAY },
        {"count"          , optional_argument, NULL, COUNT },
        {"logfile"        , optional_argument, NULL, LOG_FILE },
        {"object"         , required_argument, NULL, OBJECT },
        {0,0,0,0}
    };

    optind = 1;

    while ( ( opt = getopt_long( argc, argv, "", long_options, &long_index ) ) != -1 ) {
        switch(opt) {
		  case LOG_FILE:
			logfile = optarg;
			break;
		  case OBJECT:
			object = optarg;
		    break;
		  case COUNT:
			count = atoi(optarg);
			break;
		  case DELAY:
			delay = atoi(optarg);
			break;
		}
	}

	if ( 0 == count ) {
		forever = 1;	
	}
	if ( ! logfile.empty() ) {
		_logFP = fopen(logfile.c_str(), "w" );
		if ( ! _logFP ) {
			printf("ERROR: could not open %s\n",logfile.c_str());
			exit(-1);
		}
	}

	if ( delay ) {
		fprintf(_logFP,"sleep %d\n",delay);
		sleep(delay);
		fprintf(_logFP,"lets go\n");
	}

    // Get a context
    cntxt = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "App" );
    assert( PWR_NULL != cntxt   );

	PWR_Obj obj = PWR_CntxtGetObjByName( cntxt, object.c_str() );
    assert( PWR_NULL != obj   );

	std::string arg0 = argv[0];
	std::string exe = arg0.substr(arg0.find_last_of("/")+1); 

	kill( getppid(), SIGUSR1 );
	while ( forever || count-- ) {	
			
		PWR_AttrName names[] = { PWR_ATTR_ENERGY} ;
		value = 0;
		struct timeval tvStart,tvEnd;
		double start = getTime();
    	retval = PWR_ObjAttrGetValue( obj, PWR_ATTR_ENERGY, &value, &ts );
	
		double stop = getTime();
    	if( retval != PWR_RET_SUCCESS ) {
			fprintf(stderr,"ERROR: PWR_ObjAttrGetValue() failed, retval=%d\n",retval);
			exit(-1);
		}


		fprintf(_logFP,"%s: \'%s\' value=%.1lf, latency %f\n",
					exe.c_str(), object.c_str(), value/1000000, stop - start);
		fflush(_logFP);
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
