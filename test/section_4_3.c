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

int section_4_3_test( void )
{
    int rc, num, i;
    PWR_Cntxt cntxt;
    PWR_Grp grp = NULL, dup = NULL, un = NULL, in = NULL, diff = NULL;
    PWR_Obj self, obj;

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

    rc = PWR_GrpDuplicate( grp, &dup );
    printf( "\tPWR_GrpDuplicate - self group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: duplicating a group failed\n" );
        return rc;
    }

    rc = PWR_GrpUnion( grp, dup, &un );
    printf( "\tPWR_GrpUnion - self group and duplicate group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: the union of two groups failed\n" );
        return rc;
    }

    rc = PWR_GrpIntersection( grp, dup, &in );
    printf( "\tPWR_GrpIntersection - self group and duplicate group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: the intersection of two groups failed\n" );
        return rc;
    }

    rc = PWR_GrpDifference( grp, dup, &diff );
    printf( "\tPWR_GrpDifference - self group and duplicate group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: the difference of two groups failed\n" );
        return rc;
    }

    if( dup != NULL ) {
        rc = PWR_GrpDestroy( dup );
        printf( "\tPWR_GrpDestroy - duplicate group: %s\n", RESULT( rc ) );
        if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
            printf( "\t\tError: destroying the duplicate group failed\n" );
            return rc;
        }
    }

    if( un != NULL ) {
        rc = PWR_GrpDestroy( un );
        printf( "\tPWR_GrpDestroy - union group: %s\n", RESULT( rc ) );
        if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
            printf( "\t\tError: destroying the union group failed\n" );
            return rc;
        }
    }

    if( in != NULL ) {
        rc = PWR_GrpDestroy( in );
        printf( "\tPWR_GrpDestroy - intersection group: %s\n", RESULT( rc ) );
        if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
            printf( "\t\tError: destroying the intersection group failed\n" );
            return rc;
        }
    }

    if( diff != NULL ) {
        rc = PWR_GrpDestroy( diff );
        printf( "\tPWR_GrpDestroy - difference group: %s\n", RESULT( rc ) );
        if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
            printf( "\t\tError: destroying the difference group failed\n" );
            return rc;
        }
    }

    num = PWR_GrpGetNumObjs( grp );
    printf( "\tPWR_GrpGetNumObjs - self group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: retrieving number of objects in a group failed\n" );
        return rc;
    }

    for( i=0; i<num; i++ ) {
        rc = PWR_GrpGetObjByIndx( grp, i, &obj );
        printf( "\tPWR_GrpGetObjByIndx - index %d of group: %s\n", i, RESULT( rc ) );
        if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
            printf( "\t\tError: retrieving object %d from a group failed\n", i );
            return rc;
        }
    }

    rc = PWR_GrpRemoveObj( grp, self );
    printf( "\tPWR_GrpRemoveObj - self from self group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: removing a object to a group failed\n" );
        return rc;
    }

    // TODO - PWR_CntxtGetGrpByName( cntxt, "Predefined", name );

    rc = PWR_GrpDestroy( grp );
    printf( "\tPWR_GrpDestroy - self group: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destroying a group failed\n" );
        return rc;
    }

    rc = PWR_CntxtDestroy( cntxt );
    printf( "\tPWR_CntxtDestroy - application context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI context failed\n" );
        return rc;
    }

    return 0;
}

