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


void dump_type_objects( PWR_Obj self, PWR_ObjType type )
{
    unsigned int i, j;
    size_t size;
    PWR_Grp grp;
    PWR_Obj obj;

    if( (grp=PWR_ObjGetChildren( self )) == (PWR_Grp)0x0 )
        return;

    size = PWR_GrpGetNumObjs( grp );
    for( i = 0; i < size; i++ ) {
        obj = PWR_GrpGetObjByIndx( grp, i );
        if( type == PWR_OBJ_INVALID || PWR_ObjGetType( obj ) == type ) {
            printf( "%s %s", PWR_ObjGetName( obj ),
                    PWR_ObjGetTypeString( type ) );
            for( j = 0; j < TOTAL_NUM_PWR_ATTRS; j++ ) {
                printf( " %s", PWR_AttrGetTypeString( j ) );
            }
            printf( "\n" );
        }
        dump_type_objects( obj, type );
    }
}

int main( int argc, char* argv[] )
{
    PWR_Obj self;
    PWR_Cntxt cntxt;
    PWR_Grp grp;

    int option;
    unsigned int i, j, samples = 1, freq = 1, numattrs = 0;
    PWR_ObjType type = PWR_OBJ_INVALID;

    static char usage[] =
        "usage: %s [-t type] [-h]\n";

    while( (option=getopt( argc, argv, "t:h" )) != -1 )
        switch( option ) {
            case 't':
                switch( optarg[0] ) {
                    case 'P':
                        type = PWR_OBJ_PLATFORM;
                        break;
                    case 'A':
                        type = PWR_OBJ_CABINET;
                        break;
                    case 'B':
                        type = PWR_OBJ_BOARD;
                        break;
                    case 'N':
                        type = PWR_OBJ_NODE;
                        break;
                    case 'S':
                        type = PWR_OBJ_SOCKET;
                        break;
                    case 'C':
                        type = PWR_OBJ_CORE;
                        break;
                    case 'M':
                        type = PWR_OBJ_MEM;
                        break;
                    case 'I':
                        type = PWR_OBJ_NIC;
                        break;
                    default:
                        printf( "Error: unsupported object type (try P B N S C M I)\n" );
                        return -1;
                }
                break;
            case 'h':
            case '?':
                fprintf( stderr, usage, argv[0] );
                exit( 1 );
        }

    if( (cntxt=PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "Application" )) == 0x0 ) {
        printf( "Error: initialization of PowerAPI context failed\n" );
        return -1;
    }

    if( (self=PWR_CntxtGetEntryPoint( cntxt )) == 0x0 ) {
        printf( "Error: getting self from PowerAPI context failed\n" );
        return -1;
    }

    dump_type_objects( self, type );

    return 0;
}
