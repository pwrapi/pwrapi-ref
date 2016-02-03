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
#include <getopt.h>
#include <assert.h>
#include "logger.h"
#include "debug.h"

using namespace PWR_Logger;

static void initArgs( int argc, char* argv[], Args* );

Logger::Logger( int argc, char* argv[] ) :
	m_logFP( stdout ), m_delay( 0 )
{
	initArgs( argc, argv, &m_args );

    setenv( (m_args.name + "POWERAPI_CONFIG" ).c_str(),
                m_args.pwrApiConfig.c_str(), 0 );
    setenv( (m_args.name + "POWERAPI_ROOT").c_str(),
                m_args.pwrApiRoot.c_str(), 0 );

    DBGX("name=%s\n",m_args.name.c_str());
    DBGX("config=%s\n",m_args.pwrApiConfig.c_str());
    DBGX("root=%s\n",m_args.pwrApiRoot.c_str());

    if ( ! m_args.pwrApiServer.empty() ) {
        setenv( (m_args.name + "POWERAPI_SERVER").c_str(),
                m_args.pwrApiServer.c_str(), 0 );
    }
    if ( ! m_args.pwrApiServerPort.empty() ) {
        setenv( (m_args.name + "POWERAPI_SERVER_PORT").c_str(),
                m_args.pwrApiServerPort.c_str(), 0 );
    }

    PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_ADMIN , m_args.name.c_str(), &m_ctx );
    assert(m_ctx);

    PWR_CntxtGetObjByName( m_ctx, m_args.objectName.c_str(), &m_obj );
    assert( PWR_NULL != m_obj );

	if ( ! m_args.delay.empty() ) {
		m_delay = atoi( m_args.delay.c_str() );
	}

    if ( ! m_args.logfile.empty() ) {
        m_logFP = fopen(m_args.logfile.c_str(), "w" );
        if ( ! m_logFP ) {
            printf("ERROR: could not open %s\n",m_args.logfile.c_str());
            exit(-1);
        }
    }

}
Logger::~Logger()
{
	PWR_CntxtDestroy(m_ctx);
}

//#ifdef __MACH__
#include <sys/time.h>
#define CLOCK_REALTIME 0

int clock_gettime(int /*clk_id*/, struct timespec* t) {
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
//#endif

static double getTime() {
    struct timespec spec;
    int rc = clock_gettime( CLOCK_REALTIME, &spec );
    assert( 0 == rc );

    return (spec.tv_sec * 1000) + ((double) spec.tv_nsec / 1000000.0);
}


int Logger::work()
{
    if ( m_delay ) {
        fprintf(m_logFP,"sleep %d\n",m_delay);
        fflush(m_logFP);
        sleep(m_delay);
        fprintf(m_logFP,"lets go\n");
        fflush(m_logFP);
    }
	while( 1 ) {
        double value = 0;
		PWR_Time ts;
        double start = getTime();
        int retval = PWR_ObjAttrGetValue( m_obj, PWR_ATTR_ENERGY, &value, &ts );

        double stop = getTime();
        if( retval != PWR_RET_SUCCESS ) {
            fprintf(stderr,"ERROR: PWR_ObjAttrGetValue() failed, retval=%d\n",retval);
            exit(-1);
        }


        fprintf(m_logFP,"Logger: \'%s\' value=%.1lf, latency %f\n",
                     m_args.objectName.c_str(), value/1000000, stop - start);
        fflush(m_logFP);
        sleep(1);
	}
	return 0;
}

static void print_usage() {
    printf("Logger::%s()\n",__func__);
}

static void initArgs( int argc, char* argv[], Args* args )
{
    int opt = 0;
    int long_index = 0;
    enum { RTR_PORT, RTR_HOST,
            PWRAPI_CONFIG, PWRAPI_ROOT,
            PWRAPI_SERVER, PWRAPI_SERVER_PORT, NAME,
			OBJECT, LOGFILE, COUNT, DELAY  };
    static struct option long_options[] = {
        {"object"           , required_argument, NULL, OBJECT },
        {"logfile"          , required_argument, NULL, LOGFILE },
        {"name"             , required_argument, NULL, NAME },
        {"count"            , required_argument, NULL, COUNT },
        {"delay"            , required_argument, NULL, DELAY },
        {"pwrApiConfig"     , required_argument, NULL, PWRAPI_CONFIG },
        {"pwrApiRoot"       , required_argument, NULL, PWRAPI_ROOT },
        {"pwrApiServer"     , required_argument, NULL, PWRAPI_SERVER },
        {"pwrApiServerPort" , required_argument, NULL, PWRAPI_SERVER_PORT },
        {0,0,0,0}
    };

    optind=1;

    while ( ( opt = getopt_long( argc, argv, "", long_options, &long_index ) ) != -1 ) {
        switch(opt) {
          case LOGFILE:
            args->logfile = optarg;
            break;
          case OBJECT:
            args->objectName = optarg;
            break;
          case NAME:
            args->name = optarg;
            break;
          case COUNT:
            args->count = optarg;
            break;
          case DELAY:
            args->delay = optarg;
            break;
          case PWRAPI_CONFIG:
            args->pwrApiConfig = optarg;
            break;
          case PWRAPI_ROOT:
            args->pwrApiRoot = optarg;
            break;
          case PWRAPI_SERVER:
            args->pwrApiServer = optarg;
            break;
          case PWRAPI_SERVER_PORT:
            args->pwrApiServerPort = optarg;
            break;
          default:
            print_usage();
        }
    }
}
