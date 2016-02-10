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

int main( int argc, char* argv[] )
{
    int rc = PWR_RET_SUCCESS;

    printf( "Instantiating compliance test for Section 4.1\n");
    rc |= section_4_1_test( );
    printf( "Results from compliance test for Section 4.1: %s\n", 
            (rc == PWR_RET_SUCCESS ) ? "SUCCESS" : "FAILURE" );

    printf( "Instantiating compliance test for Section 4.2\n");
    rc |= section_4_2_test( );
    printf( "Results from compliance test for Section 4.2: %s\n", 
            (rc == PWR_RET_SUCCESS ) ? "SUCCESS" : "FAILURE" );

    printf( "Instantiating compliance test for Section 4.3\n");
    rc |= section_4_3_test( );
    printf( "Results from compliance test for Section 4.3: %s\n", 
            (rc == PWR_RET_SUCCESS ) ? "SUCCESS" : "FAILURE" );

    printf( "Instantiating compliance test for Section 4.4\n");
    rc |= section_4_4_test( );
    printf( "Results from compliance test for Section 4.4: %s\n", 
            (rc == PWR_RET_SUCCESS ) ? "SUCCESS" : "FAILURE" );

    printf( "Instantiating compliance test for Section 4.5\n");
    rc |= section_4_5_test( );
    printf( "Results from compliance test for Section 4.5: %s\n", 
            (rc == PWR_RET_SUCCESS ) ? "SUCCESS" : "FAILURE" );

    printf( "Instantiating compliance test for Section 4.6\n");
    rc |= section_4_6_test( );
    printf( "Results from compliance test for Section 4.6: %s\n", 
            (rc == PWR_RET_SUCCESS ) ? "SUCCESS" : "FAILURE" );

    printf( "Instantiating compliance test for Section 4.7\n");
    rc |= section_4_7_test( );
    printf( "Results from compliance test for Section 4.7: %s\n", 
            (rc == PWR_RET_SUCCESS ) ? "SUCCESS" : "FAILURE" );

    return rc;
}
