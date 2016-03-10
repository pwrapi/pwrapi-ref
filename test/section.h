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

#ifndef SECTION_H
#define SECTION_H

#define RESULT( X ) \
    (X >= PWR_RET_SUCCESS ) ? "SUCCESS" : \
    ((X == PWR_RET_NOT_IMPLEMENTED) ? "NOT IMPLEMENTED" : "FAILURE")

int section_4_1_test( void );
int section_4_2_test( void );
int section_4_3_test( void );
int section_4_4_test( void );
int section_4_5_test( void );
int section_4_6_test( void );
int section_4_7_test( void );

#endif
