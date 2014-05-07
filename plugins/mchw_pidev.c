#include "mchw_pidev.h"
#include "piapi.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

static void *cntx = 0x0;
static piapi_port_t port;
static piapi_sample_t counter;

static int pidev_reading = 0;

static void pidev_callback( piapi_sample_t *sample )
{
    if( sample->total && sample->number == sample->total )
        pidev_reading = 0;

    counter = *sample;
}

static int pidev_parse( char *initstr, unsigned int *saddr, unsigned int *sport, unsigned int *port )
{
    int shift = 24;
    char *token;

    *saddr = 0;
    token = strtok( initstr, "." );
    while( shift >= 0 ) {
        *saddr |= ( atoi(token) << shift );
        if( (token = strtok( NULL, "." )) == 0x0) {
            printf( "Error: invalid server IP address in initialization string %s\n", initstr );
            return -1;
        }
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

    return 0;
}

int mchw_pidev_init( mchw_dev_t *dev, char *initstr )
{
    unsigned int saddr = 0, sport = 0, port = 0;

    if( initstr == 0x0 && pidev_parse(initstr, &saddr, &sport, &port) < 0 ) {
        printf( "Error: invalid monitor and control hardware initialization string\n" );
        return -1;
    }

    if( piapi_init( &cntx, PIAPI_MODE_PROXY, pidev_callback, saddr, sport ) < 0 ) {
        printf( "Error: powerinsight hardware initialization failed\n" );
        return -1;
    }

    return 0;
}

int mchw_pidev_final( mchw_dev_t *dev )
{
    if( piapi_destroy( &cntx ) < 0 ) {
        printf( "Error: powerinsight hardware finalization failed\n" );
        return -1;
    }

    return 0;
}

int mchw_pidev_open( mchw_dev_t dev )
{
    return 0;
}

int mchw_pidev_close( mchw_dev_t dev )
{
    return 0;
}

int mchw_pidev_read( mchw_dev_t dev, unsigned int arraysize,
	mchw_read_type_t type[], float reading[], unsigned long long *timestamp )
{
    unsigned int i;

    pidev_reading = 1;
    if( piapi_counter( cntx, port ) < 0 ) {
        printf( "Error: powerinsight hardware read failed\n" );
        return -1;
    }
    while( pidev_reading ) sched_yield();

    for( i = 0; i < arraysize; i++ ) {
        switch( type[i] ) {
            case MCHW_VOLTS:
                reading[i] = counter.raw.volts;
                break;
            case MCHW_AMPS:
                reading[i] = counter.raw.amps;
                break;
            case MCHW_WATTS:
                reading[i] = counter.raw.watts;
                break;
            default:
                printf( "Error: unknown MCHW reading type requested\n" );
                return -1;
        }
    }
    *timestamp = counter.time_sec*1000000000ULL + counter.time_usec*1000;

    return 0;
}

int mchw_pidev_time( mchw_dev_t dev, unsigned long long *time )
{
    mchw_pidev_read( dev, 0, 0x0, 0x0, time );
    return 0;
}

