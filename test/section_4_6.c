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

int section_4_6_test( void )
{
    int rc, i;
    PWR_Cntxt cntxt;
    PWR_Obj self;
    PWR_Stat stat;
    double val;
    PWR_TimePeriod tp;
    PWR_AttrName attrs[] = { PWR_ATTR_POWER, PWR_ATTR_ENERGY };
    double vals[NUM_ATTR(attrs)];
    PWR_TimePeriod tps[NUM_ATTR(attrs)];

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "Application", &cntxt );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI context failed\n" );
        return -1;
    }

	rc = PWR_CntxtGetEntryPoint( cntxt, &self );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: getting self from PowerAPI context failed\n" );
        return -1;
    }

    rc = PWR_ObjCreateStat( self, PWR_ATTR_POWER, PWR_ATTR_STAT_AVG, stat );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: creating stat for object failed\n" );
        return -1;
    }

    rc = PWR_StatStart( stat );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: starting stat for object failed\n" );
        return -1;
    }

    rc = PWR_StatStop( stat );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: stoping stat for object failed\n" );
        return -1;
    }

    rc = PWR_StatGetValue( stat, &val, &tp );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: stoping stat for object failed\n" );
        return -1;
    }

    rc = PWR_StatGetValues( stat, vals, tps );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: stoping stat for object failed\n" );
        return -1;
    }

#if 0
    rc = PWR_StatGetReduce( stat, PWR_ATTR_STAT_MIN, &i, vals, tps );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: stoping stat for object failed\n" );
        return -1;
    }
#endif

    rc = PWR_StatClear( stat );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: clearing stat for object failed\n" );
        return -1;
    }

    rc = PWR_CntxtDestroy( cntxt );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: destruction of PowerAPI context failed\n" );
        return -1;
    }

    return 0;
}
