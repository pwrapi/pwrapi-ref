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

int section_4_4_test( void )
{
    int rc;
    PWR_Cntxt cntxt;
    PWR_Obj self;
    double val = 0.0;
    PWR_Time ts = 0;
    PWR_Time *tss = 0x0;
    double *vals = 0x0;
    PWR_AttrName attrs[] = { PWR_ATTR_POWER, PWR_ATTR_ENERGY };
    PWR_Status stat;
    PWR_AttrAccessError err; 

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

    rc = PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, &val, &ts );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: object attribute retrieval failed\n" );
        return -1;
    }

    rc = PWR_ObjAttrSetValue( self, PWR_ATTR_POWER, &val );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: object attribute control failed\n" );
        return -1;
    }

    rc = PWR_ObjAttrGetValues( self, NUM_ATTR(attrs), attrs, vals, tss, &stat );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: object attributes retrieval failed\n" );
        return -1;
    }

    rc = PWR_ObjAttrSetValues( self, NUM_ATTR(attrs), attrs, vals, &stat );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: object attributes control failed\n" );
        return -1;
    }

    rc = PWR_ObjAttrIsValid( self, PWR_ATTR_POWER );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: object attribute valid check failed\n" );
        return -1;
    }

	rc = PWR_StatusCreate( &stat );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: creating status failed\n" );
        return -1;
    }

    rc = PWR_StatusPopError( stat, &err );  
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: popping error failed\n" );
        return -1;
    }

    rc = PWR_StatusClear( stat );  
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: clearing error failed\n" );
        return -1;
    }

    rc = PWR_StatusDestroy( cntxt );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: destruction of status failed\n" );
        return -1;
    }

    rc = PWR_CntxtDestroy( cntxt );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: destruction of PowerAPI context failed\n" );
        return -1;
    }

    return 0;
}
