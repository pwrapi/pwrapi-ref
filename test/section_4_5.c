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

int section_4_5_test( void )
{
    int rc, i;
    PWR_Cntxt cntxt;
    PWR_Obj self;
    double val = 0.0;
    unsigned int num_meta = 0;
    char str[PWR_MAX_STRING_LEN];

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "Application", &cntxt );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI context failed\n" );
        return -1;
    }

	rc = PWR_CntxtGetEntryPoint( cntxt, &self );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "\t\tError: getting self from PowerAPI context failed\n" );
        return -1;
    }

#if 0
    rc = PWR_ObjAttrGetMeta( self, PWR_ATTR_POWER, PWR_MD_SAMPLE_RATE, &val );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "\t\tError: getting meta data failed\n" );
        return -1;
    }

    rc = PWR_ObjAttrSetMeta( self, PWR_ATTR_POWER, PWR_MD_SAMPLE_RATE, &val );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "\t\tError: setting meta data failed\n" );
        return -1;
    }

    rc = PWR_ObjAttrGetMeta( self, PWR_ATTR_PSTATE, PWR_MD_NUM, &num_meta );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "\t\tError: getting number of meta data failed\n" );
        return -1;
    }
    
    for( i=0; i<num_meta; i++ ) {
        rc = PWR_MetaValueAtIndex( self, PWR_ATTR_POWER, i, &val, str );
        printf( "\t\tError: retrieval of meta data at index %d failed\n", i );
        return -1;
    }
#endif

    rc = PWR_CntxtDestroy( cntxt );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI context failed\n" );
        return -1;
    }

    return 0;
}
