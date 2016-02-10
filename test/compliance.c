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

    rc |= section_4_1_test( );
    printf( "Compliance test for Section 4.1: %s\n", 
            (rc != PWR_RET_SUCCESS ) ? "SUCCESS" : "FAILURE" );

    rc |= section_4_2_test( );
    printf( "Compliance test for Section 4.2: %s\n", 
            (rc != PWR_RET_SUCCESS ) ? "SUCCESS" : "FAILURE" );

    return rc;
}
