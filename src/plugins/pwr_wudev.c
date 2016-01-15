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

#include "pwr_wudev.h"
#include "pwr_dev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>

typedef struct {
    int fd;
} pwr_wudev_t;
#define PWR_WUDEV(X) ((pwr_wudev_t *)(X))

typedef struct {
    pwr_wudev_t *dev;
} pwr_wufd_t;
#define PWR_WUFD(X) ((pwr_wufd_t *)(X))

static plugin_devops_t devops = {
    .open         = pwr_wudev_open,
    .close        = pwr_wudev_close,
    .read         = pwr_wudev_read,
    .write        = pwr_wudev_write,
    .readv        = pwr_wudev_readv,
    .writev       = pwr_wudev_writev,
    .time         = pwr_wudev_time,
    .clear        = pwr_wudev_clear,
#if 0
    .stat_get     = pwr_dev_stat_get,
    .stat_start   = pwr_dev_stat_start,
    .stat_stop    = pwr_dev_stat_stop,
    .stat_clear   = pwr_dev_stat_clear,
#endif
    .private_data = 0x0
};

static int wudev_parse( const char *initstr, char *port, int *baud )
{
    char *token;

    DBGP( "Info: received initialization string %s\n", initstr );

    if( (token = strtok( (char *)initstr, ":" )) == 0x0 ) {
        fprintf( stderr, "Error: missing serial port seperator in initialization string %s\n", initstr );
        return -1;
    }
    strcpy( port, token );

    if( (token = strtok( NULL, ":" )) == 0x0 ) {
        fprintf( stderr, "Error: missing serial baud seperator in initialization string %s\n", initstr );
        return -1;
    }
    *baud = atoi(token);

    DBGP( "Info: extracted initialization string (PORT=%s, BAUD=%d)\n", port, *baud);

    return 0;
}

static int wudev_open( const char *port, int baud )
{
    struct termios opts;
    int fd;
    speed_t rate;

    if( (fd=open( port, O_RDWR | O_NOCTTY | O_NDELAY)) == -1 )
        return -1;

    if( tcgetattr( fd, &opts ) < 0 )
        return -1;

    switch( baud ) {
        case 4800:
            rate = B4800;
            break;
        case 9600:
            rate = B9600;
            break;
        case 19200:
            rate = B19200;
            break;
        case 38400:
            rate = B38400;
            break;
        case 57600:
            rate = B57600;
            break;
        case 115200:
            rate = B115200;
            break;
        default:
            fprintf( stderr, "Error: unsupported baud rate of %d\n", baud );
            return -1;
    }

    cfsetispeed( &opts, rate );
    cfsetospeed( &opts, rate );

    opts.c_cflag &= ~PARENB;
    opts.c_cflag &= ~CSTOPB;
    opts.c_cflag &= ~CSIZE;
    opts.c_cflag |= CS8;

    opts.c_cflag &= ~CRTSCTS;
    opts.c_cflag |= CREAD | CLOCAL;
    opts.c_iflag &= ~(IXON | IXOFF | IXANY);

    opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    opts.c_oflag &= ~OPOST;

    opts.c_cc[VMIN] = 0;
    opts.c_cc[VTIME] = 20;

    if( tcsetattr( fd, TCSANOW, &opts) < 0 )
        return -1;

    return fd;
}

static int wudev_write( int fd, const char *str )
{
    int len = strlen( str );

    if( len > 0 && write( fd, str, len ) != len )
        return -1;

    return 0;
}

static int wudev_read( int fd, char *buf, char term )
{
    char c;
    int i = 0, n;

    do {
        n = read( fd, &c, 1 );

        if( n == -1 )
            return -1;

        if( n == 0 ) {
            usleep(5 * 1000);
            continue;
        }

        if( c != '\r' && c != '\n' )
            buf[i++] = c;
    } while( c != term );

    buf[i] = '\0';
    return 0;
}

static long long wudev_extract( const char *buf, int pos )
{
    char *token;
    int i;

    for( i = 0; i < pos; i++ )
        if( (token = strtok( i ? NULL : (char *)buf, "," )) == 0x0 )
            return 0;

    return atol(token);
}

plugin_devops_t *pwr_wudev_init( const char *initstr )
{
    char port[256] = "";
    int baud = 0;

    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    *dev = devops;

    dev->private_data = malloc( sizeof(pwr_wudev_t) );
    bzero( dev->private_data, sizeof(pwr_wudev_t) );

    DBGP( "Info: initializing PWR Wattsup device\n" );

    if( initstr == 0x0 || wudev_parse( initstr, port, &baud ) < 0 ) {
        fprintf( stderr, "Error: invalid monitor and control hardware initialization string\n" );
        return 0x0;
    }

    if( (PWR_WUDEV(dev->private_data)->fd = wudev_open( port, baud )) < 0 ) {
        fprintf( stderr, "Error: wattsup hardware initialization failed\n" );
        return 0x0;
    }

    return dev;
}

int pwr_wudev_final( plugin_devops_t *dev )
{
    DBGP( "Info: finalizing PWR Wattsup device\n" );

    close( PWR_WUDEV(dev->private_data)->fd );
    free( dev->private_data );
    free( dev );

    return 0;
}

pwr_fd_t pwr_wudev_open( plugin_devops_t *dev, const char *openstr )
{
    pwr_fd_t *fd = malloc( sizeof(pwr_wufd_t) );
    bzero( fd, sizeof(pwr_wufd_t) );

    DBGP( "Info: opening PWR Wattsup descriptor\n" );

    PWR_WUFD(fd)->dev = PWR_WUDEV(dev->private_data);

    return fd;
}

int pwr_wudev_close( pwr_fd_t fd )
{
    DBGP( "Info: closing PWR Wattsup descriptor\n" );

    PWR_WUFD(fd)->dev = 0x0;
    free( fd );

    return 0;
}

int pwr_wudev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    char buf[256] = "";
    struct timeval tv;

    DBGP( "Info: reading from PWR Wattsup device\n" );
 
    if( wudev_write( (PWR_WUFD(fd)->dev)->fd, "#L,W,3,E,0,1;" ) == -1 ) {
        fprintf( stderr, "Error: command write to Wattsup device failed\n" );
        return -1;
    }

    if( wudev_read( (PWR_WUFD(fd)->dev)->fd, buf, ';' ) != 0 ) {
        fprintf( stderr, "Error: reading from Wattsup device failed\n" );
        return -1;
    }

    if( strlen( buf ) > 0 ) {
        DBGP( "Info: read buffer is %s\n", buf );

        switch( attr ) {
            case PWR_ATTR_VOLTAGE:
                *((double *)value) = wudev_extract( buf, 5 ) / 10.0;
                break;
            case PWR_ATTR_CURRENT:
                *((double *)value) = wudev_extract( buf, 6 ) / 1000.0;
                break;
            case PWR_ATTR_POWER:
                *((double *)value) = wudev_extract( buf, 4 ) / 10.0;
                break;
            case PWR_ATTR_ENERGY:
                *((double *)value) = wudev_extract( buf, 7 ) / 10.0 * 3600;
                break;
            default:
                fprintf( stderr, "Warning: unknown PWR reading attr (%u) requested\n", attr );
                break;
        }
        gettimeofday( &tv, NULL );
        *timestamp = tv.tv_sec*1000000000ULL + tv.tv_usec*1000;
    }
    
    return 0;
}

int pwr_wudev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    DBGP( "Info: writing to PWR Wattsup device\n" );

    switch( attr ) {
        default:
            fprintf( stderr, "Warning: unknown PWR reading attr (%u)\n", attr );
            break;

        DBGP( "Info: setting of type %u with value %lf\n",
                    attr, *((double *)value) );
    }

    return 0;
}

int pwr_wudev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_wudev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );
    
    return 0;
}

int pwr_wudev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_wudev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

    return 0;
}

int pwr_wudev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;

    DBGP( "Info: getting time from PWR Wattsup device\n" );

    return pwr_wudev_read( fd, PWR_ATTR_POWER, &value, sizeof(double), timestamp);
}

int pwr_wudev_clear( pwr_fd_t fd )
{
    DBGP( "Info: clearing PWR Wattsup device\n" );

    return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_wudev_init,
    .final  = pwr_wudev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}
