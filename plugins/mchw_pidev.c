#include "mchw_pidev.h"
#include "piapi.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

static int pidev_verbose = 1;

typedef struct {
    void *cntx;
    piapi_port_t port;
} mchw_pidev_t;
#define MCHW_PIDEV(X) ((mchw_pidev_t *)(X))

static piapi_sample_t pidev_counter;
static int pidev_reading;

static void pidev_callback( piapi_sample_t *sample )
{
    pidev_counter = *sample;

    if( sample->total && sample->number == sample->total )
        pidev_reading = 0;
}

static int pidev_parse( const char *initstr, unsigned int *saddr, unsigned int *sport, unsigned int *port )
{
    int shift = 24;
    char *token;

    if( pidev_verbose )
        printf( "Info: received initialization string %s\n", initstr );

    *saddr = 0;
    while( shift >= 0 ) {
        if( (token = strtok( (shift!=24) ? NULL : (char *)initstr, (shift!=0) ? "." : ":" )) == 0x0 ) {
            printf( "Error: invalid server IP address in initialization string %s\n", initstr );
            return -1;
        }
        *saddr |= ( atoi(token) << shift );
        shift -= 8;
    }

    if( (token = strtok( NULL, ":" )) == 0x0 ) {
        printf( "Error: missing server port separator in initialization string %s\n", initstr );
        return -1;
    }
    *sport = atoi(token);

    if( (token = strtok( NULL, ":" )) == 0x0 ) {
        printf( "Error: missing sensor port separator in initialization string %s\n", initstr );
        return -1;
    }
    *port = atoi(token);

    if( pidev_verbose )
        printf( "Info: extracted initialization string (SADDR=%08x, SPORT=%u, PORT=%u)\n", *saddr, *sport, *port );

    return 0;
}

pwr_dev_t mchw_pidev_open( const char *initstr )
{
    unsigned int saddr = 0, sport = 0, port = 0;

    pwr_dev_t *dev = malloc( sizeof(mchw_pidev_t) );
    bzero( dev, sizeof(mchw_pidev_t) );

    if( pidev_verbose )
        printf( "Info: opening MCHW PowerInsight device\n" );

    if( initstr == 0x0 || pidev_parse(initstr, &saddr, &sport, &port) < 0 ) {
        printf( "Error: invalid monitor and control hardware initialization string\n" );
        return 0x0;
    }

    if( piapi_init( &(MCHW_PIDEV(dev)->cntx), PIAPI_MODE_PROXY, pidev_callback, saddr, sport ) < 0 ) {
        printf( "Error: powerinsight hardware initialization failed\n" );
        return 0x0;
    }

    return dev;
}

int mchw_pidev_close( pwr_dev_t dev )
{
    if( pidev_verbose )
        printf( "Info: closing MCHW PowerInsight device\n" );

    if( piapi_destroy( &(MCHW_PIDEV(dev)->cntx) ) < 0 ) {
        printf( "Error: powerinsight hardware finalization failed\n" );
        return -1;
    }

    free( dev );

    return 0;
}


int mchw_pidev_read( pwr_dev_t dev, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    while( pidev_reading ) sched_yield();
    pidev_reading = 1;
    if( piapi_counter( MCHW_PIDEV(dev)->cntx, MCHW_PIDEV(dev)->port ) < 0 ) {
        printf( "Error: powerinsight hardware read failed\n" );
        return -1;
    }
    while( pidev_reading ) sched_yield();

    if( len != sizeof(double) ) {
        printf( "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(double) );
        return -1;
    }

    switch( attr ) {
        case PWR_ATTR_VOLTAGE:
            *((double *)value) = (double)pidev_counter.raw.volts;
            break;
        case PWR_ATTR_CURRENT:
            *((double *)value) = (double)pidev_counter.raw.amps;
            break;
        case PWR_ATTR_POWER:
            *((double *)value) = (double)pidev_counter.raw.watts;
            break;
        case PWR_ATTR_MIN_POWER:
            *((double *)value) = (double)pidev_counter.min.watts;
            break;
        case PWR_ATTR_MAX_POWER:
            *((double *)value) = (double)pidev_counter.max.watts;
            break;
        case PWR_ATTR_ENERGY:
            *((double *)value) = (double)pidev_counter.energy;
            break;
        default:
            printf( "Warning: unknown MCHW reading attr (%u) requested\n", attr );
            break;
    }
    *timestamp = pidev_counter.time_sec*1000000000ULL + 
            pidev_counter.time_usec*1000;

    if( pidev_verbose )
        printf( "Info: reading of type %u at time %llu with value %lf\n",
                attr, *(unsigned long long *)timestamp, *(double *)value );

    return 0;
}

int mchw_pidev_write( pwr_dev_t dev, PWR_AttrName attr, void *value, unsigned int len )
{
    if( pidev_verbose )
        printf( "Info: setting of type %u with value %lf\n",
                attr, *(double *)value );

    return 0;
}

int mchw_pidev_readv( pwr_dev_t dev, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    while( pidev_reading ) sched_yield();
    pidev_reading = 1;
    if( piapi_counter( MCHW_PIDEV(dev)->cntx, MCHW_PIDEV(dev)->port ) < 0 ) {
        printf( "Error: powerinsight hardware read failed\n" );
        return -1;
    }
    while( pidev_reading ) sched_yield();

    for( i = 0; i < arraysize; i++ ) {
        switch( attrs[i] ) {
            case PWR_ATTR_VOLTAGE:
                *((double *)values+i) = pidev_counter.raw.volts;
                break;
            case PWR_ATTR_CURRENT:
                *((double *)values+i) = pidev_counter.raw.amps;
                break;
            case PWR_ATTR_POWER:
                *((double *)values+i) = pidev_counter.raw.watts;
                break;
            case PWR_ATTR_MIN_POWER:
                *((double *)values+i) = pidev_counter.min.watts;
                break;
            case PWR_ATTR_MAX_POWER:
                *((double *)values+i) = pidev_counter.max.watts;
                break;
            case PWR_ATTR_ENERGY:
                *((double *)values+i) = pidev_counter.energy;
                break;
            default:
                printf( "Warning: unknown MCHW reading attr (%u) requested at position %u\n", attrs[i], i );
                break;
        }
        timestamp[i] = pidev_counter.time_sec*1000000000ULL + 
                       pidev_counter.time_usec*1000;

        if( pidev_verbose )
            printf( "Info: reading of type %u at time %llu with value %lf\n",
                    attrs[i], *(unsigned long long *)timestamp[i], *((double *)(values+i)) );
    }

    return 0;
}

int mchw_pidev_writev( pwr_dev_t dev, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ ) {
        switch( attrs[i] ) {
            default:
                printf( "Warning: unknown MCHW reading attr (%u) requested at position %u\n", attrs[i], i );
                break;
        }

        if( pidev_verbose )
            printf( "Info: setting of type %u with value %lf\n",
                    attrs[i], *((double *)(values+i)) );
    }

    return 0;
}

int mchw_pidev_time( pwr_dev_t dev, PWR_Time *timestamp )
{
    double value;

    if( pidev_verbose )
        printf( "Info: getting time from MCHW PowerInsight device\n" );

    return mchw_pidev_read( dev, PWR_ATTR_POWER, &value, sizeof(double), timestamp );
}

int mchw_pidev_clear( pwr_dev_t dev )
{
    if( pidev_verbose )
        printf( "Info: clearing MCHW PowerInsight device\n" );

    return 0;
} 

static plugin_dev_t dev = {
    .open   = mchw_pidev_open,
    .close  = mchw_pidev_close,
    .read   = mchw_pidev_read,
    .write  = mchw_pidev_write,
    .readv  = mchw_pidev_readv,
    .writev = mchw_pidev_writev,
    .time   = mchw_pidev_time,
    .clear  = mchw_pidev_clear
};

plugin_dev_t* getDev() {
    return &dev;
}
