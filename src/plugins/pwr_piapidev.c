/*
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power APIAPI Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#include "pwr_piapidev.h"
#include "pwr_dev.h"
#include "piapi.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

typedef struct {
    void *cntx;
} pwr_piapidev_t;
#define PWR_PIAPIDEV(X) ((pwr_piapidev_t *)(X))

typedef struct {
    pwr_piapidev_t *dev;
    piapi_port_t port;
} pwr_pifd_t;
#define PWR_PIAPIFD(X) ((pwr_pifd_t *)(X))

static plugin_devops_t devops = {
    .open         = pwr_piapidev_open,
    .close        = pwr_piapidev_close,
    .read         = pwr_piapidev_read,
    .write        = pwr_piapidev_write,
    .readv        = pwr_piapidev_readv,
    .writev       = pwr_piapidev_writev,
    .time         = pwr_piapidev_time,
    .clear        = pwr_piapidev_clear,
#if 0
    .stat_get     = pwr_dev_stat_get,
    .stat_start   = pwr_dev_stat_start,
    .stat_stop    = pwr_dev_stat_stop,
    .stat_clear   = pwr_dev_stat_clear,
#endif
    .private_data = 0x0
};

static piapi_sample_t piapidev_counter;
static int piapidev_reading;

static void piapidev_callback( piapi_sample_t *sample )
{
    piapidev_counter = *sample;

    DBGP( "Sample on port %d:\n", sample->port);
    DBGP( "\tsample       - %u of %u\n", sample->number, sample->total );
    DBGP( "\ttime         - %f\n", sample->time_sec+sample->time_usec/1000000.0 );
    DBGP( "\tvolts        - %f\n", sample->raw.volts );
    DBGP( "\tamps         - %f\n", sample->raw.amps );
    DBGP( "\twatts        - %f\n", sample->raw.watts );

    DBGP( "\tavg volts    - %f\n", sample->avg.volts );
    DBGP( "\tavg amps     - %f\n", sample->avg.amps );
    DBGP( "\tavg watts    - %f\n", sample->avg.watts );

    DBGP( "\tmin volts    - %f\n", sample->min.volts );
    DBGP( "\tmin amps     - %f\n", sample->min.amps );
    DBGP( "\tmin watts    - %f\n", sample->min.watts );

    DBGP( "\tmax volts    - %f\n", sample->max.volts );
    DBGP( "\tmax amps     - %f\n", sample->max.amps );
    DBGP( "\tmax watts    - %f\n", sample->max.watts );

    DBGP( "\ttotal time   - %f\n", sample->time_total );
    DBGP( "\ttotal energy - %f\n", sample->energy );
	
    if( sample->total && sample->number == sample->total )
        piapidev_reading = 0;
}

static int piapidev_parse( const char *initstr, unsigned int *saddr, unsigned int *sport )
{
    int shift = 24;
    char *token;

    DBGP( "Info: received initialization string %s\n", initstr );

    *saddr = 0;
    while( shift >= 0 ) {
        if( (token = strtok( (shift!=24) ? NULL : (char *)initstr, (shift!=0) ? "." : ":" )) == 0x0 ) {
            fprintf( stderr, "Error: invalid server IP address in initialization string %s\n", initstr );
            return -1;
        }
        *saddr |= ( atoi(token) << shift );
        shift -= 8;
    }

    if( (token = strtok( NULL, ":" )) == 0x0 ) {
        fprintf( stderr, "Error: missing server port separator in initialization string %s\n", initstr );
        return -1;
    }
    *sport = atoi(token);

    DBGP( "Info: extracted initialization string (SADDR=%08x, SPORT=%u)\n", *saddr, *sport );

    return 0;
}

plugin_devops_t *pwr_piapidev_init( const char *initstr )
{
    unsigned int saddr = 0, sport = 0;

    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    *dev = devops;

    dev->private_data = malloc( sizeof(pwr_piapidev_t) );
    bzero( dev->private_data, sizeof(pwr_piapidev_t) );

    DBGP( "Info: initializing PWR PowerInsight device\n" );

    if( initstr == 0x0 || piapidev_parse(initstr, &saddr, &sport) < 0 ) {
        fprintf( stderr, "Error: invalid monitor and control hardware initialization string\n" );
        return 0x0;
    }

    if( piapi_init( &(PWR_PIAPIDEV(dev->private_data)->cntx), PIAPI_MODE_PROXY, piapidev_callback, saddr, sport, 0 ) < 0 ) {
        fprintf( stderr, "Error: powerinsight hardware initialization failed\n" );
        return 0x0;
    }

    return dev;
}

int pwr_piapidev_final( plugin_devops_t *dev )
{
    DBGP( "Info: finaling PWR PowerInsight device\n" );

    if( piapi_destroy( &(PWR_PIAPIDEV(dev->private_data)->cntx) ) < 0 ) {
        fprintf( stderr, "Error: powerinsight hardware finalization failed\n" );
        return -1;
    }

    free( dev->private_data );
    free( dev );

    return 0;
}

pwr_fd_t pwr_piapidev_open( plugin_devops_t *dev, const char *openstr )
{
    char *token;

    pwr_fd_t *fd = malloc( sizeof(pwr_pifd_t) );
    bzero( fd, sizeof(pwr_pifd_t) );

    DBGP( "Info: opening PWR PowerInsight descriptor\n" );

    PWR_PIAPIFD(fd)->dev = PWR_PIAPIDEV(dev->private_data);

    if( openstr == 0x0 || (token = strtok( (char *)openstr, ":" )) == 0x0 ) {
        fprintf( stderr, "Error: missing sensor port separator in initialization string %s\n", openstr );
        return 0x0;
    }
    PWR_PIAPIFD(fd)->port = atoi(token);

    DBGP( "Info: extracted initialization string (PORT=%u)\n", PWR_PIAPIFD(fd)->port );

    return fd;
}

int pwr_piapidev_close( pwr_fd_t fd )
{
    DBGP( "Info: closing PWR PowerInsight descriptor\n" );

    PWR_PIAPIFD(fd)->dev = 0x0;
    free( fd );

    return 0;
}

int pwr_piapidev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    while( piapidev_reading ) sched_yield();
    piapidev_reading = 1;
    DBGP( "Info: reading counter for port %d\n", PWR_PIAPIFD(fd)->port );
    if( piapi_counter( (PWR_PIAPIFD(fd)->dev)->cntx, PWR_PIAPIFD(fd)->port ) < 0 ) {
        fprintf( stderr, "Error: powerinsight hardware read failed\n" );
        return -1;
    }
    while( piapidev_reading ) sched_yield();

    if( len != sizeof(double) ) {
        fprintf( stderr, "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(double) );
        return -1;
    }

    switch( attr ) {
        case PWR_ATTR_VOLTAGE:
            *((double *)value) = (double)piapidev_counter.raw.volts;
            break;
        case PWR_ATTR_CURRENT:
            *((double *)value) = (double)piapidev_counter.raw.amps;
            break;
        case PWR_ATTR_POWER:
            *((double *)value) = (double)piapidev_counter.raw.watts;
            break;
        case PWR_ATTR_POWER_LIMIT_MIN:
            *((double *)value) = (double)piapidev_counter.min.watts;
            break;
        case PWR_ATTR_POWER_LIMIT_MAX:
            *((double *)value) = (double)piapidev_counter.max.watts;
            break;
        case PWR_ATTR_ENERGY:
            *((double *)value) = (double)piapidev_counter.energy;
            break;
        default:
            fprintf( stderr, "Warning: unknown PWR reading attr (%u) requested\n", attr );
            break;
    }
    *timestamp = piapidev_counter.time_sec*1000000000ULL + 
            piapidev_counter.time_usec*1000;

    DBGP( "Info: reading of type %u at time %llu with value %lf\n",
        attr, *(unsigned long long *)timestamp, *(double *)value );

    return 0;
}

int pwr_piapidev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
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

int pwr_piapidev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    while( piapidev_reading ) sched_yield();
    piapidev_reading = 1;
    DBGP( "Info: reading counter for port %d\n", PWR_PIAPIFD(fd)->port );
    if( piapi_counter( (PWR_PIAPIFD(fd)->dev)->cntx, PWR_PIAPIFD(fd)->port ) < 0 ) {
        fprintf( stderr, "Error: powerinsight hardware read failed\n" );
        return -1;
    }
    while( piapidev_reading ) sched_yield();

    for( i = 0; i < arraysize; i++ ) {
        switch( attrs[i] ) {
            case PWR_ATTR_VOLTAGE:
                *((double *)values+i) = (double)piapidev_counter.raw.volts;
                break;
            case PWR_ATTR_CURRENT:
                *((double *)values+i) = (double)piapidev_counter.raw.amps;
                break;
            case PWR_ATTR_POWER:
                *((double *)values+i) = (double)piapidev_counter.raw.watts;
                break;
            case PWR_ATTR_POWER_LIMIT_MIN:
                *((double *)values+i) = (double)piapidev_counter.min.watts;
                break;
            case PWR_ATTR_POWER_LIMIT_MAX:
                *((double *)values+i) = (double)piapidev_counter.max.watts;
                break;
            case PWR_ATTR_ENERGY:
                *((double *)values+i) = (double)piapidev_counter.energy;
                break;
            default:
                fprintf( stderr, "Warning: unknown PWR reading attr (%u) requested\n", attrs[i] );
                break;
        }
        timestamp[i] = piapidev_counter.time_sec*1000000000ULL + 
             piapidev_counter.time_usec*1000;

        DBGP( "Info: reading of type %u at time %llu with value %lf\n",
            attrs[i], *((unsigned long long *)timestamp+i), *((double *)values+i) );

        status[i] = pwr_piapidev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );
    }

    return 0;
}

int pwr_piapidev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_piapidev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

    return 0;
}

int pwr_piapidev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;

    DBGP( "Info: getting time from PWR PowerInsight device\n" );

    return pwr_piapidev_read( fd, PWR_ATTR_POWER, &value, sizeof(double), timestamp );
}

int pwr_piapidev_clear( pwr_fd_t fd )
{
    DBGP( "Info: clearing PWR PowerInsight device\n" );

    return 0;
} 

static plugin_dev_t dev = {
    .init   = pwr_piapidev_init, 
    .final  = pwr_piapidev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

