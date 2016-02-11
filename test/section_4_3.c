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

    rc = PWR_GrpCreate( cntxt, &grp );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: creating a group failed\n" );
        return -1;
    }

    rc = PWR_GrpAddObj( grp, self );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: adding a object to a group failed\n" );
        return -1;
    }

#if 0
    rc = PWR_GrpDuplicate( grp, &dup );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: duplicating a group failed\n" );
        return -1;
    }

    rc = PWR_GrpDestroy( dup );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: destroying the duplicate group failed\n" );
        return -1;
    }

    rc = PWR_GrpUnion( grp, dup, &un );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: the union of two groups failed\n" );
        return -1;
    }

    rc = PWR_GrpDestroy( un );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: destroying the union group failed\n" );
        return -1;
    }

    rc = PWR_GrpIntersection( grp, dup, &in );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: the intersection of two groups failed\n" );
        return -1;
    }

    rc = PWR_GrpDestroy( in );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: destroying the intersection group failed\n" );
        return -1;
    }

    rc = PWR_GrpDifference( grp, dup, &diff );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: the difference of two groups failed\n" );
        return -1;
    }

    rc = PWR_GrpDestroy( diff );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: destroying the difference group failed\n" );
        return -1;
    }
#endif

    num = PWR_GrpGetNumObjs( grp );
    if( rc < PWR_RET_SUCCESS ) {
        printf( "Error: retrieving number of objects in a group failed\n" );
        return -1;
    }

    for( i=0; i<num; i++ ) {
        rc = PWR_GrpGetObjByIndx( grp, i, &obj );
        if( rc != PWR_RET_SUCCESS ) {
            printf( "Error: retrieving object %d from a group failed\n", i );
            return -1;
        }
    }

    rc = PWR_GrpRemoveObj( grp, self );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: removing a object to a group failed\n" );
        return -1;
    }

    // TODO - PWR_CntxtGetGrpByName( cntxt, "Predefined", name );

    rc = PWR_GrpDestroy( grp );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: destroying a group failed\n" );
        return -1;
    }

    rc = PWR_CntxtDestroy( cntxt );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: destruction of PowerAPI context failed\n" );
        return -1;
    }

    return 0;
}

