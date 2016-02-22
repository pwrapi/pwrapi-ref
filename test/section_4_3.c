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

int section_4_3_test( void )
{
    int rc, num, i;
    PWR_Cntxt cntxt;
    PWR_Grp grp, dup, un, in, diff;
    PWR_Obj self, obj;

    printf( "\tPWR_CntxtInit - application context\n" );
	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "Application", &cntxt );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI context failed\n" );
        return rc;
    }

    printf( "\tPWR_CntxtGetEntryPoint\n" );
	rc = PWR_CntxtGetEntryPoint( cntxt, &self );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: getting self from PowerAPI context failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpCreate\n" );
    rc = PWR_GrpCreate( cntxt, &grp );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: creating a group failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpAddObj - self object\n" );
    rc = PWR_GrpAddObj( grp, self );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: adding a object to a group failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpDuplicate - self group\n" );
    rc = PWR_GrpDuplicate( grp, &dup );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: duplicating a group failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpUnion - self group and duplicate group\n" );
    rc = PWR_GrpUnion( grp, dup, &un );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: the union of two groups failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpIntersection - self group and duplicate group\n" );
    rc = PWR_GrpIntersection( grp, dup, &in );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: the intersection of two groups failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpDifference - self group and duplicate group\n" );
    rc = PWR_GrpDifference( grp, dup, &diff );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: the difference of two groups failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpDestroy - duplicate group\n" );
    rc = PWR_GrpDestroy( dup );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destroying the duplicate group failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpDestroy - union group\n" );
    rc = PWR_GrpDestroy( un );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destroying the union group failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpDestroy - intersection group\n" );
    rc = PWR_GrpDestroy( in );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destroying the intersection group failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpDestroy - difference group\n" );
    rc = PWR_GrpDestroy( diff );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destroying the difference group failed\n" );
        return rc;
    }

    printf( "\tPWR_GrpGetNumObjs - self group\n" );
    num = PWR_GrpGetNumObjs( grp );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: retrieving number of objects in a group failed\n" );
        return rc;
    }

    for( i=0; i<num; i++ ) {
        printf( "\tPWR_GrpGetObjByIndx - index %d of group\n", i );
        rc = PWR_GrpGetObjByIndx( grp, i, &obj );
        if( rc < PWR_RET_SUCCESS ) {
            printf( "\t\tError: retrieving object %d from a group failed\n", i );
            return rc;
        }
    }

    printf( "\tPWR_GrpRemoveObj - self from self group\n" );
    rc = PWR_GrpRemoveObj( grp, self );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: removing a object to a group failed\n" );
        return rc;
    }

    // TODO - PWR_CntxtGetGrpByName( cntxt, "Predefined", name );

    printf( "\tPWR_GrpDestroy - self group\n" );
    rc = PWR_GrpDestroy( grp );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destroying a group failed\n" );
        return rc;
    }

    printf( "\tPWR_CntxtDestroy - application context\n" );
    rc = PWR_CntxtDestroy( cntxt );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI context failed\n" );
        return rc;
    }

    return 0;
}

