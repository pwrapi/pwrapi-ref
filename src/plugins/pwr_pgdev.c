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

#define USE_SYSTIME

#include "pwr_pgdev.h"
#include "pwr_dev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef USE_SYSTIME
#include <sys/time.h>
#endif

#include <IntelPowerGadget/EnergyLib.h>

typedef struct {
    int num_nodes;
    int num_msrs;
} pwr_pgdev_t;
#define PWR_PGDEV(X) ((pwr_pgdev_t *)(X))

typedef struct {
    pwr_pgdev_t *dev;
    int node;
    int gpu;
} pwr_pgfd_t;
#define PWR_PGFD(X) ((pwr_pgfd_t *)(X))

static plugin_devops_t devops = {
    .open         = pwr_pgdev_open,
    .close        = pwr_pgdev_close,
    .read         = pwr_pgdev_read,
    .write        = pwr_pgdev_write,
    .readv        = pwr_pgdev_readv,
    .writev       = pwr_pgdev_writev,
    .time         = pwr_pgdev_time,
    .clear        = pwr_pgdev_clear,
#if 0
    .stat_get     = pwr_dev_stat_get,
    .stat_start   = pwr_dev_stat_start,
    .stat_stop    = pwr_dev_stat_stop,
    .stat_clear   = pwr_dev_stat_clear,
#endif
    .private_data = 0x0
};

static int pgdev_parse( const char *openstr, int *node, int *gpu )
{
    char *token;

    DBGP( "Info: received initialization string %s\n", openstr );

    *node = 0;
    if( (token = strtok( (char *)openstr, ":" )) == 0x0 ) {
        fprintf( stderr, "Error: missing server port separator in initialization string %s\n", openstr );
        return -1;
    }
    *node = atoi(token);

    *gpu = 0;
    if( (token = strtok( NULL, ":" )) == 0x0 ) {
        fprintf( stderr, "Error: missing server port separator in initialization string %s\n", openstr );
        return -1;
    }
    *gpu = atoi(token);

    DBGP( "Info: extracted initialization string (NODE=%d, GPU=%d)\n", *node, *gpu );

    return 0;
}

plugin_devops_t *pwr_pgdev_init( const char *initstr )
{
    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    *dev = devops;

    dev->private_data = malloc( sizeof(pwr_pgdev_t) );
    bzero( dev->private_data, sizeof(pwr_pgdev_t) );

    DBGP( "Info: initializing PWR PowerGadget device\n" );

    IntelEnergyLibInitialize();
    StartLog("/tmp/PowerGadgetLog.csv");

    PWR_PGDEV(dev->private_data)->num_nodes = 0;
    PWR_PGDEV(dev->private_data)->num_msrs = 0;
    GetNumNodes( &(PWR_PGDEV(dev->private_data)->num_nodes) );
    GetNumMsrs( &(PWR_PGDEV(dev->private_data)->num_msrs) );

    return dev;
}

int pwr_pgdev_final( plugin_devops_t *dev )
{
    DBGP( "Info: finalizing PWR PowerGadget device\n" );

    StopLog();
                                                
    free( dev->private_data );
    free( dev );

    return 0;
}

pwr_fd_t pwr_pgdev_open( plugin_devops_t *dev, const char *openstr )
{
    pwr_fd_t *fd = malloc( sizeof(pwr_pgfd_t) );
    bzero( fd, sizeof(pwr_pgfd_t) );

    DBGP( "Info: opening PWR PowerGadget descriptor\n" );

    PWR_PGFD(fd)->dev = PWR_PGDEV(dev->private_data);
    if( openstr == 0x0 || pgdev_parse(openstr, &(PWR_PGFD(fd)->node), &(PWR_PGFD(fd)->gpu)) < 0 ) {
        fprintf( stderr, "Error: invalid monitor and control hardware open string\n" );
        return 0x0;
    }

    return fd;
}

int pwr_pgdev_close( pwr_fd_t fd )
{
    DBGP( "Info: closing PWR PowerGadget device\n" );

    PWR_PGFD(fd)->dev = 0x0;
    free( fd );

    return 0;
}

int pwr_pgdev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    int func, val = 0;
    int i, n;
    double data[3];
#ifndef USE_SYSTIME
    struct timeval tv;
#else
    uint64_t systime;
#endif

    DBGP( "Info: reading from PWR PowerGadget device\n" );

#ifndef USE_SYSTIME
    gettimeofday( &tv, NULL );
#else
    GetSysTime( &systime );
#endif

    switch( attr ) {
        case PWR_ATTR_ENERGY:
            ReadSample();
            for( i = 0; i < (PWR_PGFD(fd)->dev)->num_msrs; i++ ) {
                if( GetMsrFunc( i, &func ) < 0 )
                    fprintf( stderr, "Warning: reading msr %d function %d\n", i, func );
                else if( func == MSR_FUNC_POWER ) {
                        if( GetPowerData( 0, i, data, &n ) < 0 )
                            fprintf( stderr, "Warning: reading msr %d function %d\n", i, func );
                        *((double *)value) = data[1];
                        break;
                }
            }
            break;
        case PWR_ATTR_POWER:
            ReadSample();
            for( i = 0; i < (PWR_PGFD(fd)->dev)->num_msrs; i++ ) {
                GetMsrFunc( i, &func );
                if( func == MSR_FUNC_POWER ) {
                    GetPowerData( 0, i, data, &n );
                    *((double *)value) = data[0];
                    break;
                }
            }
            break;
        case PWR_ATTR_FREQ:
            if( !PWR_PGFD(fd)->gpu ) {
                if( GetIAFrequency( PWR_PGFD(fd)->node, &val ) < 0 )
                    fprintf( stderr, "Warning: reading node %d frequency\n", PWR_PGFD(fd)->node );
            } else if( GetGTFrequency( &val ) )
                fprintf( stderr, "Warning: reading gpu frequency\n" );
            *((double *)value) = val * 10000000.0;
            break;
        case PWR_ATTR_TEMP:
            GetTemperature( PWR_PGFD(fd)->node, &val );
            *((double *)value) = val;
            break;
        default:
            fprintf( stderr, "Warning: unknown PWR reading attr (%u) requested\n", attr );
            break;
    }
#ifndef USE_SYSTIME
    *timestamp = tv.tv_sec*1000000000ULL + tv.tv_usec*1000;
#else
    *timestamp = (systime>>32)*1000000000ULL + ((uint32_t)systime);
#endif

    DBGP( "Info: reading of type %u at time %llu with value %lf\n",
          attr, *(unsigned long long *)timestamp, *(double *)value );

    return 0;
}

int pwr_pgdev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    DBGP( "Info: writing to PWR PowerGadget device\n" );

    return 0;
}

int pwr_pgdev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    DBGP( "Info: reading from PWR PowerGadget device\n" );

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_pgdev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );

    return 0;
}

int pwr_pgdev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    DBGP( "Info: writing to PWR PowerGadget device\n" );

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_pgdev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

    return 0;
}

int pwr_pgdev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;

    DBGP( "Info: reading time from PWR PowerGadget device\n" );

    return pwr_pgdev_read( fd, PWR_ATTR_POWER, &value, sizeof(double), timestamp );;
}

int pwr_pgdev_clear( pwr_fd_t fd )
{
    return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_pgdev_init,
    .final  = pwr_pgdev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}
