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

int section_4_7_test( void )
{
    int rc;

    rc = PWR_GetMajorVersion( );
    printf( "\tPWR_GetMajorVersion: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < 0 ) {
        printf( "\t\tError: retrieving PowerAPI major version failed\n" );
        return rc;
    }

    rc = PWR_GetMinorVersion( );
    printf( "\tPWR_GetMinorVersion: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < 0 ) {
        printf( "\t\tError: retrieving PowerAPI minor version failed\n" );
        return rc;
    }

    return 0;
}
