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
    printf( "\tPWR_CntxtInit - application context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI application context failed\n" );
        return rc;
    }

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_MC, "MonitorControl", &role_mc );
    printf( "\tPWR_CntxtInit - monitor and control context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI monitor and control context failed\n" );
        return rc;
    }

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_OS, "OperatingSystem", &role_os );
    printf( "\tPWR_CntxtInit - operating system context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI operating system context failed\n" );
        return rc;
    }

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_USER, "", &role_user );
    printf( "\tPWR_CntxtInit - user context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI user context failed\n" );
        return rc;
    }

	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_RM, "ResourceManager", &role_rm );
    printf( "\tPWR_CntxtInit - resource manager context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI resource manager context failed\n" );
        return rc;
    }

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_ADMIN, "Administrator", &role_admin );
    printf( "\tPWR_CntxtInit - administrator context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI administrator context failed\n" );
        return rc;
    }

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_MGR, "HPCSManager", &role_mgr );
    printf( "\tPWR_CntxtInit - HPCS manager context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI HPCS manager context failed\n" );
        return rc;
    }

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_ACC, "Accounting", &role_acc );
    printf( "\tPWR_CntxtInit - accounting context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI accounting context failed\n" );
        return rc;
    }

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_INVALID, "Invalid", &role_inv );
    printf( "\tPWR_CntxtInit - invalid context: %s\n", RESULT( rc ) );
    if( rc == PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI invalid context succeeded\n" );
        return rc;
    }

    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_NOT_SPECIFIED, "NotSpecified", &role_ns );
    printf( "\tPWR_CntxtInit - not specified context: %s\n", RESULT( rc ) );
    if( rc == PWR_RET_SUCCESS ) {
        printf( "\t\tError: initialization of PowerAPI not specified context succeeded\n" );
        return rc;
    }

    rc = PWR_CntxtDestroy( role_app );
    printf( "\tPWR_CntxtDestroy - application context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI application context failed\n" );
        return rc;
    }

    rc = PWR_CntxtDestroy( role_mc );
    printf( "\tPWR_CntxtDestroy - monitor and control context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI monitor and control context failed\n" );
        return rc;
    }

    rc = PWR_CntxtDestroy( role_os );
    printf( "\tPWR_CntxtDestroy - operating system context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI operating system context failed\n" );
        return rc;
    }

    rc = PWR_CntxtDestroy( role_user );
    printf( "\tPWR_CntxtDestroy - user context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI user context failed\n" );
        return rc;
    }

    rc = PWR_CntxtDestroy( role_rm );
    printf( "\tPWR_CntxtDestroy - resource manager context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI resource manager context failed\n" );
        return rc;
    }

    rc = PWR_CntxtDestroy( role_admin );
    printf( "\tPWR_CntxtDestroy - administrator context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI administrator context failed\n" );
        return rc;
    }

    rc = PWR_CntxtDestroy( role_mgr );
    printf( "\tPWR_CntxtDestroy - HPCS manager context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI HPCS manager context failed\n" );
        return rc;
    }

    rc = PWR_CntxtDestroy( role_acc );
    printf( "\tPWR_CntxtDestroy - accounting context: %s\n", RESULT( rc ) );
    if( rc != PWR_RET_NOT_IMPLEMENTED && rc < PWR_RET_SUCCESS ) {
        printf( "\t\tError: destruction of PowerAPI accounting context failed\n" );
        return rc;
    }

    return 0;
}
