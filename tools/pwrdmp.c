/*
 * Copyright 2014-2015 Sandia Corporation. Under the terms of Contract
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

void dump_object( PWR_Obj self )
{
    PWR_ObjType type = PWR_GetObjType( self );
    char typestr[20] = "";

    switch( type ) {
        case PWR_OBJ_PLATFORM:
            strcpy( typestr, "Platform");
            break;
        case PWR_OBJ_BOARD:
            strcpy( typestr, "Board");
            break;
        case PWR_OBJ_NODE:
            strcpy( typestr, "Node");
            break;
        case PWR_OBJ_SOCKET:
            strcpy( typestr, "Socket");
            break;
        case PWR_OBJ_CORE:
            strcpy( typestr, "Core");
            break;
        case PWR_OBJ_MEM:
            strcpy( typestr, "Memory");
            break;
        case PWR_OBJ_NIC:
            strcpy( typestr, "NIC");
            break;
        default:
            strcpy( typestr, "Invalid");
            break;
    }

    switch( type ) {
        case PWR_ATTR_NAME:
            strcpy( attrstr, "Name");
            break;
        case PWR_ATTR_FREQ:
            strcpy( attrstr, "Frequency");
            break;
        case PWR_ATTR_MAX_PCAP:
            strcpy( attrstr, "Maximum PowerCap");
            break;
        case PWR_ATTR_MIN_PCAP:
            strcpy( attrstr, "Minimum PowerCap");
            break;
        case PWR_ATTR_MAX_POWER:
            strcpy( attrstr, "Maximum Power");
            break;
        case PWR_ATTR_MIN_POWER:
            strcpy( attrstr, "Minimum Power");
            break;
        case PWR_ATTR_AVG_POWER:
            strcpy( attrstr, "Average Power");
            break;
        case PWR_ATTR_POWER:
            strcpy( attrstr, "Power");
            break;
        case PWR_ATTR_VOLTAGE:
            strcpy( attrstr, "Voltage");
            break;
        case PWR_ATTR_CURRENT:
            strcpy( attrstr, "Current");
            break;
        case PWR_ATTR_ENERGY:
            strcpy( attrstr, "Energy");
            break;
        case PWR_ATTR_TEMP:
            strcpy( attrstr, "Temperature");
            break;
        case PWR_ATTR_PSTATE:
            strcpy( attrstr, "P-State");
            break;
        case PWR_ATTR_CSTATE:
            strcpy( attrstr, "C-State");
            break;
        case PWR_ATTR_SSTATE:
            strcpy( attrstr, "S-State");
            break;
        case PWR_ATTR_FREQ_LIMIT_MAX:
            strcpy( attrstr, "Frequency Limit Maximum");
            break;
        case PWR_ATTR_FREQ_LIMIT_MIN:
            strcpy( attrstr, "Frequency Limit Minimum");
            break;
        case PWR_ATTR_NOT_SPECIFIED:
            strcpy( attrstr, "Not Specified");
            break;
        case PWR_ATTR_INVALID:
        default:
            strcpy( attrstr, "Invalid");
            break;
    }

    printf( "%s %s %s\n", PWR_GetObjName( self ), typestr, attrstr ); 
}

void dump_type_objects( PWR_Obj self, PWR_ObjType type )
{
    unsigned int i, j, count = 0;
    size_t size;
    PWR_Grp grp;
    PWR_Obj obj;

    if( (grp=PWR_ObjGetChildren( self )) == (PWR_Grp)0x0 ) {
        printf( "Error: getting child objects from PowerAPI context failed\n" );
        return (PWR_Grp)0x0;
    }
    size = PWR_GrpGetNumObjs( grp );
    for( i = 0; i < size; i++ ) {
        obj = PWR_GrpGetObjByIndx( grp, i );
        for( j = 0; j < count; j++ ) {
            if( PWR_ObjGetType( obj ) == type )
                dump_object( obj );
            dump_type_object( obj, type );
        }
    }

    return grp;
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

    if( type != PWR_OBJ_INVALID )
        dump_type_objects( self, type );
    else {
        printf( "Error: getting core objects failed\n" );
        return -1;
    }

    return 0;
}
