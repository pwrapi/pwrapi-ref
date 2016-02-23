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

typedef int (*test_t)( void );

int check( char *desc, test_t test )
{
    int rc, i;

    printf( "Instantiating compliance check for %s\n", desc );
    rc = test( );
    printf( "Results from compliance check for %s: %s\n", desc, RESULT( rc ) );

    return (rc != PWR_RET_SUCCESS);
}

int main( int argc, char* argv[] )
{
    int test = PWR_RET_SUCCESS;

    test |= check( "Section 4.1", section_4_1_test );
    test |= check( "Section 4.2", section_4_2_test );
    test |= check( "Section 4.3", section_4_3_test );
    test |= check( "Section 4.4", section_4_4_test );
    test |= check( "Section 4.5", section_4_5_test );
    test |= check( "Section 4.6", section_4_6_test );
    test |= check( "Section 4.7", section_4_7_test );

    return test;
}
