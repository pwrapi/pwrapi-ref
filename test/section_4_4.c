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
#include "section.h"

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
    PWR_Grp grp;
    PWR_Obj self;
    double val = 0.0;
    PWR_Time ts = 0;
    PWR_AttrName attrs[] = { PWR_ATTR_POWER, PWR_ATTR_ENERGY };
    PWR_Time tss[NUM_ATTR(attrs)];
    double vals[NUM_ATTR(attrs)];
    PWR_Status stat;
    PWR_AttrAccessError err; 

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "Application", &cntxt );
    printf( "\tPWR_CntxtInit - application context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI context failed\n" );
        return rc;
    }

    rc = PWR_CntxtGetEntryPoint( cntxt, &self );
    printf( "\tPWR_CntxtGetEntryPoint: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: getting self from PowerAPI context failed\n" );
        return rc;
    }

    rc = PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, &val, &ts );
    printf( "\tPWR_ObjAttrGetValue - PWR_ATTR_POWER of self: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: object attribute retrieval failed\n" );
        return rc;
    }

    rc = PWR_ObjAttrSetValue( self, PWR_ATTR_POWER, &val );
    printf( "\tPWR_ObjAttrSetValue - PWR_ATTR_POWER of self: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: object attribute control failed\n" );
        return rc;
    }

#if 0
    rc = PWR_ObjAttrGetValues( self, NUM_ATTR(attrs), attrs, vals, tss, &stat );
    printf( "\tPWR_ObjAttrGetValues - PWR_ATTR_POWER, PWR_ATTR_ENERGY of self: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: object attributes retrieval failed\n" );
        return rc;
    }

    rc = PWR_ObjAttrSetValues( self, NUM_ATTR(attrs), attrs, vals, &stat );
    printf( "\tPWR_ObjAttrSetValues - PWR_ATTR_POWER, PWR_ATTR_ENERGY of self: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: object attributes control failed\n" );
        return rc;
    }
#endif

    rc = PWR_ObjAttrIsValid( self, PWR_ATTR_POWER );
    printf( "\tPWR_ObjAttrIsValid - PWR_ATTR_POWER of self: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: object attribute valid check failed\n" );
        return rc;
    }

    rc = PWR_GrpCreate( cntxt, &grp );
    printf( "\tPWR_GrpCreate: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: creating a group failed\n" );
        return rc;
    }

    rc = PWR_GrpAddObj( grp, self );
    printf( "\tPWR_GrpAddObj - self object: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: adding a object to a group failed\n" );
        return rc;
    }

#if 0
    rc = PWR_GrpAttrGetValue( grp, PWR_ATTR_POWER, &val, &ts, &stat );
    printf( "\tPWR_GrpAttrGetValue - PWR_ATTR_POWER of self group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: group attribute retrieval failed\n" );
        return rc;
    }

    rc = PWR_GrpAttrSetValue( grp, PWR_ATTR_POWER, &val, &stat );
    printf( "\tPWR_GrpAttrSetValue - PWR_ATTR_POWER of self group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: group attribute control failed\n" );
        return rc;
    }

    rc = PWR_GrpAttrGetValues( grp, NUM_ATTR(attrs), attrs, vals, tss, &stat );
    printf( "\tPWR_GrpAttrGetValues - PWR_ATTR_POWER, PWR_ATTR_ENERGY of self group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: group attributes retrieval failed\n" );
        return rc;
    }

    rc = PWR_GrpAttrSetValues( grp, NUM_ATTR(attrs), attrs, vals, &stat );
    printf( "\tPWR_GrpAttrSetValues - PWR_ATTR_POWER, PWR_ATTR_ENERGY of self group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: group attributes control failed\n" );
        return rc;
    }
#endif

    rc = PWR_GrpRemoveObj( grp, self );
    printf( "\tPWR_GrpRemoveObj - self object: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: removing a object to a group failed\n" );
        return rc;
    }

    rc = PWR_GrpDestroy( grp );
    printf( "\tPWR_GrpDestroy - self group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destroying a group failed\n" );
        return rc;
    }

	rc = PWR_StatusCreate( cntxt, &stat );
    printf( "\tPWR_StatusCreate: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: creating status failed\n" );
        return rc;
    }

#if 0
    rc = PWR_StatusPopError( stat, &err );
    printf( "\tPWR_StatusPopError: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: popping error failed\n" );
        return rc;
    }
#endif

    rc = PWR_StatusClear( stat );
    printf( "\tPWR_StatusClear: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: clearing error failed\n" );
        return rc;
    }

    rc = PWR_StatusDestroy( stat );
    printf( "\tPWR_StatusDestroy: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of status failed\n" );
        return rc;
    }

    rc = PWR_CntxtDestroy( cntxt );
    printf( "\tPWR_CntxtDestroy: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI context failed\n" );
        return rc;
    }

    return 0;
}
