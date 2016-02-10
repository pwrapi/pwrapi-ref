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
    int rc;
    PWR_Cntxt cntxt;
    PWR_Grp grp;

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "Application", &cntxt );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI context failed\n" );
        return -1;
    }

    rc = PWR_GrpCreate( cntxt, &grp );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: creating a group failed\n" );
        return -1;
    }

    rc = PWR_GrpDestroy( grp );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: destroying a group failed\n" );
        return -1;
    }

    return 0;
}
