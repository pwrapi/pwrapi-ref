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

#include "pwr_pmcdev.h"
#include "pwr_dev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define PMC_COUNTER1 1
#define PMC_COUNTER2 2
#define PMC_COUNTER3 3
#define PMC_COUNTER4 4
#define PMC_COUNTER5 5
#define PMC_COUNTER6 6

typedef struct {
    int fd;
} pwr_pmcdev_t;
#define PWR_PMCDEV(X) ((pwr_pmcdev_t *)(X))

typedef struct {
    pwr_pmcdev_t *dev;
    unsigned int cpu;
} pwr_pmcfd_t;
#define PWR_PMCFD(X) ((pwr_pmcfd_t *)(X))

static plugin_devops_t devops = {
    .open         = pwr_pmcdev_open,
    .close        = pwr_pmcdev_close,
    .read         = pwr_pmcdev_read,
    .write        = pwr_pmcdev_write,
    .readv        = pwr_pmcdev_readv,
    .writev       = pwr_pmcdev_writev,
    .time         = pwr_pmcdev_time,
    .clear        = pwr_pmcdev_clear,
#if 0
    .stat_get     = pwr_dev_stat_get,
    .stat_start   = pwr_dev_stat_start,
    .stat_stop    = pwr_dev_stat_stop,
    .stat_clear   = pwr_dev_stat_clear,
#endif
    .private_data = 0x0
};

static int pmcdev_read( unsigned int cpu, unsigned int counter, double *val )
{
    char path[256] = "", strval[20] = "";
    int offset = 0;
    int fd;

    sprintf( path, "/sys/devices/system/cpu/cpu%u/pmc%u", cpu, counter );
    fd = open( path, O_RDONLY );
    if( fd < 0 ) {
        fprintf( stderr, "Error: unable to open counter file at %s\n", path );
        return -1;
    }

    while( read( fd, strval+offset, 1 ) != EOF ) {
        if( strval[offset] == ' ' ) {
            *val = atof(strval);
            close( fd );
            return 0;
        }
        offset++;
    }

    fprintf( stderr, "Error: unable to parse PM counter value\n" );
    close( fd );
    return -1;
}

static int pmcdev_write( unsigned int cpu, unsigned int counter, double val )
{
    char path[256] = "", strval[20] = "";
    int fd;

    sprintf( path, "/sys/devices/system/cpu/cpu%u/pmc%u", cpu, counter );
    fd = open( path, O_WRONLY );
    if( fd < 0 ) {
        fprintf( stderr, "Error: unable to open PM counter file at %s\n", path );
        return -1;
    }

    sprintf( strval, "%lf", val ); 
    if( write( fd, strval, strlen(strval) ) < 0 ) {
        fprintf( stderr, "Error: unable to write PM counter\n" );
        close( fd );
        return -1;
    }

    close( fd );
    return 0;
}

plugin_devops_t *pwr_pmcdev_init( const char *initstr )
{
    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    *dev = devops;

    dev->private_data = malloc( sizeof(pwr_pmcdev_t) );
    bzero( dev->private_data, sizeof(pwr_pmcdev_t) );

    DBGP( "Info: initializing PWR PMC device\n" );

    return dev;
}

int pwr_pmcdev_final( plugin_devops_t *dev )
{
    DBGP( "Info: finalizing PWR PMC device\n" );

    free( dev->private_data );
    free( dev );
    return 0;
}

pwr_fd_t pwr_pmcdev_open( plugin_devops_t *dev, const char *openstr )
{
    char *token;

    pwr_fd_t *fd = malloc( sizeof(pwr_pmcfd_t) );
    bzero( fd, sizeof(pwr_pmcfd_t) );

    DBGP( "Info: opening PWR PMC device\n" );

    PWR_PMCFD(fd)->dev = PWR_PMCDEV(dev->private_data);

    if( openstr == 0x0 || (token = strtok( (char *)openstr, ":" )) == 0x0 ) {
        fprintf( stderr, "Error: missing PMC separator in initialization string %s\n", openstr );
        return 0x0;
    }
    PWR_PMCFD(fd)->cpu = atoi(token);

    DBGP( "Info: extracted initialization string (PMC=%u)\n", PWR_PMCFD(fd)->cpu );

    return fd;
}

int pwr_pmcdev_close( pwr_fd_t fd )
{
    DBGP( "Info: closing PWR PMC device\n" );

    PWR_PMCFD(fd)->dev = 0x0;
    free( fd );

    return 0;
}

int pwr_pmcdev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    double generation;
    struct timeval tv;

    DBGP( "Info: reading from PWR PMC device\n" );

    if( len != sizeof(double) ) {
        fprintf( stderr, "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(double) );
        return -1;
    }

    switch( attr ) {
        case PWR_ATTR_ENERGY:
            if( pmcdev_read( PWR_PMCFD(fd)->cpu, PMC_COUNTER1, (double *)value) < 0 ) {
                fprintf( stderr, "Error: unable to read energy counter\n" );
                return -1;
            }
            break;
        case PWR_ATTR_POWER:
            if( pmcdev_read( PWR_PMCFD(fd)->cpu, PMC_COUNTER2, (double *)value) < 0 ) {
                fprintf( stderr, "Error: unable to read power counter\n" );
                return -1;
            }
            break;
        case PWR_ATTR_POWER_LIMIT_MAX:
            if( pmcdev_read( PWR_PMCFD(fd)->cpu, PMC_COUNTER3, (double *)value) < 0 ) {
                fprintf( stderr, "Error: unable to read power_cap counter\n" );
                return -1;
            }
            break;
        default:
            fprintf( stderr, "Warning: unknown PWR PMC reading attr (%u) requested\n", attr );
            break;
    }
    gettimeofday( &tv, NULL );
    *timestamp = tv.tv_sec*1000000000ULL + tv.tv_usec*1000;

    DBGP( "Info: reading of type %u at time %llu with value %lf\n",
        attr, *(unsigned long long *)timestamp, *(double *)value );
    return 0;
}

int pwr_pmcdev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    DBGP( "Info: writing to PWR PMC device\n" );

    if( len != sizeof(double) ) {
        fprintf( stderr, "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(double) );
        return -1;
    }

    switch( attr ) {
        case PWR_ATTR_POWER_LIMIT_MAX:
            if( pmcdev_write( PWR_PMCFD(fd)->cpu, PMC_COUNTER3, *((double *)value) ) < 0 ) {
                fprintf( stderr, "Error: unable to write power_cap counter\n" );
                return -1;
            }
            break;
        default:
            fprintf( stderr, "Warning: unknown PWR PMC writing attr (%u) requested\n", attr );
            break;
    }

    DBGP( "Info: reading of type %u with value %lf\n", attr, *(double *)value );
    return 0;
}

int pwr_pmcdev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_pmcdev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );

    return 0;
}

int pwr_pmcdev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_pmcdev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

    return 0;
}

int pwr_pmcdev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;

    DBGP( "Info: reading time from PWR PMC device\n" );

    return pwr_pmcdev_read( fd, PWR_ATTR_ENERGY, &value, sizeof(double), timestamp );;
}

int pwr_pmcdev_clear( pwr_fd_t fd )
{
    return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_pmcdev_init,
    .final  = pwr_pmcdev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}
