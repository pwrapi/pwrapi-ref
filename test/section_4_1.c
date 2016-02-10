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

int section_4_1_test( void )
{
    int rc;
    PWR_Cntxt 
        role_app, 
        role_mc, 
        role_os, 
        role_user, 
        role_rm, 
        role_admin, 
        role_mgr, 
        role_acc,
        role_inv,
        role_ns;

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "Application", &role_app );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI application context failed\n" );
        return -1;
    }

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_MC, "MonitorControl", &role_mc );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI monitor and control context failed\n" );
        return -1;
    }

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_OS, "OperatingSystem", &role_os );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI operating system context failed\n" );
        return -1;
    }

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_USER, "", &role_user );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI user context failed\n" );
        return -1;
    }

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_RM, "ResourceManager", &role_rm );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI resource manager context failed\n" );
        return -1;
    }

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_ADMIN, "Administrator", &role_admin );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI administrator context failed\n" );
        return -1;
    }

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_MGR, "HPCSManager", &role_mgr );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI HPCS manager context failed\n" );
        return -1;
    }

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_ACC, "Accounting", &role_acc );
    if( rc != PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI accounting context failed\n" );
        return -1;
    }

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_INVALID, "Invalid", &role_inv );
    if( rc == PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI invalid context succeeded\n" );
        return -1;
    }

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_NOT_SPECIFIED, "NotSpecified", &role_ns );
    if( rc == PWR_RET_SUCCESS ) {
        printf( "Error: initialization of PowerAPI not specified context succeeded\n" );
        return -1;
    }

    return 0;
}
