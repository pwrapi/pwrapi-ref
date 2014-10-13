#include "mchw_pidev.h"
#include "piapi.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

static int pidev_verbose = 0;

typedef struct {
    void *cntx;
} mchw_pidev_t;
#define MCHW_PIDEV(X) ((mchw_pidev_t *)(X))

typedef struct {
    mchw_pidev_t *dev;
    piapi_port_t port;
} mchw_pifd_t;
#define MCHW_PIFD(X) ((mchw_pifd_t *)(X))

static piapi_sample_t pidev_counter;
static int pidev_reading;

static void pidev_callback( piapi_sample_t *sample )
{
    pidev_counter = *sample;

    if( pidev_verbose ) {
        printf( "Sample on port %d:\n", sample->port);
       	printf( "\tsample       - %u of %u\n", sample->number, sample->total );
        printf( "\ttime         - %f\n", sample->time_sec+sample->time_usec/1000000.0 );
       	printf( "\tvolts        - %f\n", sample->raw.volts );
       	printf( "\tamps         - %f\n", sample->raw.amps );
       	printf( "\twatts        - %f\n", sample->raw.watts );

       	printf( "\tavg volts    - %f\n", sample->avg.volts );
       	printf( "\tavg amps     - %f\n", sample->avg.amps );
        printf( "\tavg watts    - %f\n", sample->avg.watts );

       	printf( "\tmin volts    - %f\n", sample->min.volts );
       	printf( "\tmin amps     - %f\n", sample->min.amps );
        printf( "\tmin watts    - %f\n", sample->min.watts );

       	printf( "\tmax volts    - %f\n", sample->max.volts );
       	printf( "\tmax amps     - %f\n", sample->max.amps );
        printf( "\tmax watts    - %f\n", sample->max.watts );

       	printf( "\ttotal time   - %f\n", sample->time_total );
       	printf( "\ttotal energy - %f\n", sample->energy );
    }
	
    if( sample->total && sample->number == sample->total )
        pidev_reading = 0;
}

static int pidev_parse( const char *initstr, unsigned int *saddr, unsigned int *sport )
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

    if( pidev_verbose )
        printf( "Info: extracted initialization string (SADDR=%08x, SPORT=%u)\n", *saddr, *sport );

    return 0;
}

plugin_devops_t *mchw_pidev_init( const char *initstr )
{
    unsigned int saddr = 0, sport = 0;

    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    bzero( dev, sizeof(plugin_devops_t) );

    dev->private_data = malloc( sizeof(mchw_pidev_t) );
    bzero( dev->private_data, sizeof(mchw_pidev_t) );

    if( pidev_verbose )
        printf( "Info: initializing MCHW PowerInsight device\n" );

    if( initstr == 0x0 || pidev_parse(initstr, &saddr, &sport) < 0 ) {
        printf( "Error: invalid monitor and control hardware initialization string\n" );
        return 0x0;
    }

    if( piapi_init( &(MCHW_PIDEV(dev->private_data)->cntx), PIAPI_MODE_PROXY, pidev_callback, saddr, sport ) < 0 ) {
        printf( "Error: powerinsight hardware initialization failed\n" );
        return 0x0;
    }

    return dev;
}

int mchw_pidev_final( plugin_devops_t *dev )
{
    if( pidev_verbose )
        printf( "Info: finaling MCHW PowerInsight device\n" );

    if( piapi_destroy( &(MCHW_PIDEV(dev->private_data)->cntx) ) < 0 ) {
        printf( "Error: powerinsight hardware finalization failed\n" );
        return -1;
    }

    free( dev->private_data );
    free( dev );

    return 0;
}

pwr_fd_t mchw_pidev_open( plugin_devops_t *dev, const char *openstr )
{
    char *token;

    pwr_fd_t *fd = malloc( sizeof(mchw_pifd_t) );
    bzero( fd, sizeof(mchw_pifd_t) );

    if( pidev_verbose )
        printf( "Info: opening MCHW PowerInsight descriptor\n" );

    MCHW_PIFD(fd)->dev = MCHW_PIDEV(dev->private_data);

    if( openstr == 0x0 || (token = strtok( (char *)openstr, ":" )) == 0x0 ) {
        printf( "Error: missing sensor port separator in initialization string %s\n", openstr );
        return 0x0;
    }
    MCHW_PIFD(fd)->port = atoi(token);

    if( pidev_verbose )
        printf( "Info: extracted initialization string (PORT=%u)\n", MCHW_PIFD(fd)->port );

    return fd;
}

int mchw_pidev_close( pwr_fd_t fd )
{
    if( pidev_verbose )
        printf( "Info: closing MCHW PowerInsight descriptor\n" );

    MCHW_PIFD(fd)->dev = 0x0;
    free( fd );

    return 0;
}

int mchw_pidev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    while( pidev_reading ) sched_yield();
    pidev_reading = 1;
    if( pidev_verbose )
        printf( "Info: reading counter for port %d\n", MCHW_PIFD(fd)->port );
    if( piapi_counter( MCHW_PIDEV(MCHW_PIFD(fd)->dev)->cntx, MCHW_PIFD(fd)->port ) < 0 ) {
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

int mchw_pidev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    switch( attr ) {
        default:
            printf( "Warning: unknown MCHW writing attr (%u) requested\n", attr );
            break;
    }

    if( pidev_verbose )
        printf( "Info: setting of type %u with value %lf\n",
                attr, *(double *)value );

    return 0;
}

int mchw_pidev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    while( pidev_reading ) sched_yield();
    pidev_reading = 1;
    if( piapi_counter( MCHW_PIDEV(MCHW_PIFD(fd)->dev)->cntx, MCHW_PIFD(fd)->port ) < 0 ) {
        printf( "Error: powerinsight hardware read failed\n" );
        return -1;
    }
    while( pidev_reading ) sched_yield();

    for( i = 0; i < arraysize; i++ ) {
        switch( attrs[i] ) {
            case PWR_ATTR_VOLTAGE:
                *((double *)values+i) = (double)pidev_counter.raw.volts;
                break;
            case PWR_ATTR_CURRENT:
                *((double *)values+i) = (double)pidev_counter.raw.amps;
                break;
            case PWR_ATTR_POWER:
                *((double *)values+i) = (double)pidev_counter.raw.watts;
                break;
            case PWR_ATTR_MIN_POWER:
                *((double *)values+i) = (double)pidev_counter.min.watts;
                break;
            case PWR_ATTR_MAX_POWER:
                *((double *)values+i) = (double)pidev_counter.max.watts;
                break;
            case PWR_ATTR_ENERGY:
                *((double *)values+i) = (double)pidev_counter.energy;
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

int mchw_pidev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ ) {
        switch( attrs[i] ) {
            default:
                printf( "Warning: unknown MCHW writing attr (%u) requested at position %u\n", attrs[i], i );
                break;
        }

        if( pidev_verbose )
            printf( "Info: setting of type %u with value %lf\n",
                    attrs[i], *((double *)(values+i)) );
    }

    return 0;
}

int mchw_pidev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;

    if( pidev_verbose )
        printf( "Info: getting time from MCHW PowerInsight device\n" );

    return mchw_pidev_read( fd, PWR_ATTR_POWER, &value, sizeof(double), timestamp );
}

int mchw_pidev_clear( pwr_fd_t fd )
{
    if( pidev_verbose )
        printf( "Info: clearing MCHW PowerInsight device\n" );

    return 0;
} 

static plugin_devops_t devops = {
    .open   = mchw_pidev_open,
    .close  = mchw_pidev_close,
    .read   = mchw_pidev_read,
    .write  = mchw_pidev_write,
    .readv  = mchw_pidev_readv,
    .writev = mchw_pidev_writev,
    .time   = mchw_pidev_time,
    .clear  = mchw_pidev_clear
};

static plugin_dev_t dev = {
    .init   = mchw_pidev_init, 
    .final  = mchw_pidev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

