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

    printf( "\tPWR_CntxtInit - application context: %s\n",
	    RESULT( rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "Application", &cntxt ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI context failed\n" );
        return rc;
    }

    printf( "\tPWR_CntxtGetEntryPoint: %s\n",
	    RESULT( rc = PWR_CntxtGetEntryPoint( cntxt, &self ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: getting self from PowerAPI context failed\n" );
        return rc;
    }

    printf( "\tPWR_ObjAttrGetValue - PWR_ATTR_POWER of self: %s\n",
        RESULT( rc = PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, &val, &ts ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: object attribute retrieval failed\n" );
        return rc;
    }

    printf( "\tPWR_ObjAttrSetValue - PWR_ATTR_POWER of self: %s\n",
        RESULT( rc = PWR_ObjAttrSetValue( self, PWR_ATTR_POWER, &val ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: object attribute control failed\n" );
        return rc;
    }

#if 0
    printf( "\tPWR_ObjAttrGetValues - PWR_ATTR_POWER, PWR_ATTR_ENERGY of self: %s\n",
        RESULT( rc = PWR_ObjAttrGetValues( self, NUM_ATTR(attrs), attrs, vals, tss, &stat ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: object attributes retrieval failed\n" );
        return rc;
    }

    printf( "\tPWR_ObjAttrSetValues - PWR_ATTR_POWER, PWR_ATTR_ENERGY of self: %s\n",
        RESULT( rc = PWR_ObjAttrSetValues( self, NUM_ATTR(attrs), attrs, vals, &stat ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: object attributes control failed\n" );
        return rc;
    }

    printf( "\tPWR_ObjAttrIsValid - PWR_ATTR_POWER of self: %s\n",
        RESULT( rc = PWR_ObjAttrIsValid( self, PWR_ATTR_POWER ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: object attribute valid check failed\n" );
        return rc;
    }
#endif

    printf( "\tPWR_GrpCreate: %s\n",
        RESULT( rc = PWR_GrpCreate( cntxt, &grp ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: creating a group failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpAddObj - self object: %s\n",
        RESULT( rc = PWR_GrpAddObj( grp, self ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: adding a object to a group failed\n" );
        return rc;
    }

#if 0
    printf( "\tPWR_GrpAttrGetValue - PWR_ATTR_POWER of self group: %s\n",
        RESULT( rc = PWR_GrpAttrGetValue( grp, PWR_ATTR_POWER, &val, &ts, &stat ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: group attribute retrieval failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpAttrSetValue - PWR_ATTR_POWER of self group: %s\n",
        RESULT( rc = PWR_GrpAttrSetValue( grp, PWR_ATTR_POWER, &val, &stat ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: group attribute control failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpAttrGetValues - PWR_ATTR_POWER, PWR_ATTR_ENERGY of self group: %s\n",
        RESULT( rc = PWR_GrpAttrGetValues( grp, NUM_ATTR(attrs), attrs, vals, tss, &stat ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: group attributes retrieval failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpAttrSetValues - PWR_ATTR_POWER, PWR_ATTR_ENERGY of self group: %s\n",
        RESULT( rc = PWR_GrpAttrSetValues( grp, NUM_ATTR(attrs), attrs, vals, &stat ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: group attributes control failed\n" );
        return rc;
    }
#endif

    printf( "\tPWR_GrpRemoveObj - self object: %s\n",
        RESULT( rc = PWR_GrpRemoveObj( grp, self ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: removing a object to a group failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpDestroy - self group: %s\n",
        RESULT( rc = PWR_GrpDestroy( grp ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destroying a group failed\n" );
        return rc;
    }

    printf( "\tPWR_StatusCreate: %s\n",
	    RESULT( rc = PWR_StatusCreate( &stat ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: creating status failed\n" );
        return rc;
    }

#if 0
    printf( "\tPWR_StatusPopError: %s\n",
        RESULT( rc = PWR_StatusPopError( stat, &err ) ) );  
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: popping error failed\n" );
        return rc;
    }
#endif

    printf( "\tPWR_StatusClear: %s\n",
        RESULT( rc = PWR_StatusClear( stat ) ) );  
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: clearing error failed\n" );
        return rc;
    }

    printf( "\tPWR_StatusDestroy: %s\n",
        RESULT( rc = PWR_StatusDestroy( stat ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of status failed\n" );
        return rc;
    }

    printf( "\tPWR_CntxtDestroy: %s\n",
        RESULT( rc = PWR_CntxtDestroy( cntxt ) ) );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI context failed\n" );
        return rc;
    }

    return 0;
}
