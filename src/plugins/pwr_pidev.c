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

#include "pwr_pidev.h"
#include "pwr_dev.h"
#include "pidev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

typedef struct {
    int port;
} pwr_pifd_t;
#define PWR_PIFD(X) ((pwr_pifd_t *)(X))

static plugin_devops_t devops = {
    .open         = pwr_pidev_open,
    .close        = pwr_pidev_close,
    .read         = pwr_pidev_read,
    .write        = pwr_pidev_write,
    .readv        = pwr_pidev_readv,
    .writev       = pwr_pidev_writev,
    .time         = pwr_pidev_time,
    .clear        = pwr_pidev_clear,
#if 0
    .stat_get     = pwr_dev_stat_get,
    .stat_start   = pwr_dev_stat_start,
    .stat_stop    = pwr_dev_stat_stop,
    .stat_clear   = pwr_dev_stat_clear,
#endif
    .private_data = 0x0
};

plugin_devops_t *pwr_pidev_init( const char *initstr )
{
    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    *dev = devops;

    dev->private_data = malloc( sizeof(pwr_pifd_t) );
    bzero( dev->private_data, sizeof(pwr_pifd_t) );

    DBGP( "Info: initializing PWR PowerInsight device\n" );
    if( pidev_open() < 0 ) {
        fprintf( stderr, "Error: failed to open powerinsight device on initialization" );

        free( dev->private_data );
        free( dev );

        return 0x0;
    }

    return dev;
}

int pwr_pidev_final( plugin_devops_t *dev )
{
    if( dev->private_data) free( dev->private_data );
    if( dev ) free( dev );

    DBGP( "Info: finaling PWR PowerInsight device\n" );
    if( pidev_close() < 0 ) {
        fprintf( stderr, "Error: failed to close powerinsight device on finalization" );
        return -1;
    }

    return 0;
}

pwr_fd_t pwr_pidev_open( plugin_devops_t *dev, const char *openstr )
{
    char *token;

    pwr_fd_t *fd = malloc( sizeof(pwr_pifd_t) );
    bzero( fd, sizeof(pwr_pifd_t) );

    DBGP( "Info: opening PWR PowerInsight descriptor\n" );

    if( openstr == 0x0 || (token = strtok( (char *)openstr, ":" )) == 0x0 ) {
        fprintf( stderr, "Error: missing sensor port separator in initialization string %s\n", openstr );
        return 0x0;
    }
    PWR_PIFD(fd)->port = atoi(token);

    DBGP( "Info: extracted initialization string (PORT=%u)\n", PWR_PIFD(fd)->port );

    return fd;
}

int pwr_pidev_close( pwr_fd_t fd )
{
    DBGP( "Info: closing PWR PowerInsight descriptor\n" );
    free( fd );

    return 0;
}

int pwr_pidev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    struct timeval tv;
    reading_t raw;

    DBGP( "Info: reading counter for port %d\n", PWR_PIFD(fd)->port );
    if( pidev_read( PWR_PIFD(fd)->port, &raw ) < 0 ) {
        fprintf( stderr, "Error: powerinsight hardware read failed\n" );
        return -1;
    }

    if( len != sizeof(double) ) {
        fprintf( stderr, "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(double) );
        return -1;
    }

    switch( attr ) {
        case PWR_ATTR_VOLTAGE:
            *((double *)value) = (double)raw.volt;
            break;
        case PWR_ATTR_CURRENT:
            *((double *)value) = (double)raw.amp;
            break;
        case PWR_ATTR_POWER:
            *((double *)value) = (double)raw.watt;
            break;
        default:
            fprintf( stderr, "Warning: unknown PWR reading attr (%u) requested\n", attr );
            break;
    }
    gettimeofday( &tv, NULL );
    *timestamp = tv.tv_sec*1000000000ULL + tv.tv_usec*1000;

    DBGP( "Info: reading of type %u at time %llu with value %lf\n",
        attr, *(unsigned long long *)timestamp, *(double *)value );

    return 0;
}

int pwr_pidev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    switch( attr ) {
        default:
            fprintf( stderr, "Warning: unknown PWR writing attr (%u) requested\n", attr );
            break;
    }

    DBGP( "Info: setting of type %u with value %lf\n",
        attr, *(double *)value );

    return 0;
}

int pwr_pidev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_pidev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );

    return 0;
}

int pwr_pidev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_pidev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

    return 0;
}

int pwr_pidev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;

    DBGP( "Info: getting time from PWR PowerInsight device\n" );

    return pwr_pidev_read( fd, PWR_ATTR_POWER, &value, sizeof(double), timestamp );
}

int pwr_pidev_clear( pwr_fd_t fd )
{
    DBGP( "Info: clearing PWR PowerInsight device\n" );

    return 0;
} 

static plugin_dev_t dev = {
    .init   = pwr_pidev_init, 
    .final  = pwr_pidev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

