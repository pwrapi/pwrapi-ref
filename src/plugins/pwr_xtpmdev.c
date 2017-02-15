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

#include "pwr_xtpmdev.h"
#include "pwr_dev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

typedef struct {
    int fd;
} pwr_xtpmdev_t;
#define PWR_XTPMDEV(X) ((pwr_xtpmdev_t *)(X))

typedef struct {
    pwr_xtpmdev_t *dev;
    double generation;
} pwr_xtpmfd_t;
#define PWR_XTPMFD(X) ((pwr_xtpmfd_t *)(X))

static plugin_devops_t devops = {
    .open         = pwr_xtpmdev_open,
    .close        = pwr_xtpmdev_close,
    .read         = pwr_xtpmdev_read,
    .write        = pwr_xtpmdev_write,
    .readv        = pwr_xtpmdev_readv,
    .writev       = pwr_xtpmdev_writev,
    .time         = pwr_xtpmdev_time,
    .clear        = pwr_xtpmdev_clear,
#if 0
    .stat_get     = pwr_dev_stat_get,
    .stat_start   = pwr_dev_stat_start,
    .stat_stop    = pwr_dev_stat_stop,
    .stat_clear   = pwr_dev_stat_clear,
#endif
    .private_data = 0x0
};

static int xtpmdev_read( const char *name, double *val )
{
    char path[256] = "", strval[20] = "";
    int offset = 0;
    int fd;

    sprintf( path, "/sys/cray/pm_counters/%s", name );
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

static int xtpmdev_write( const char *name, double val )
{
    char path[256] = "", strval[20] = "";
    int fd;

    sprintf( path, "/sys/cray/pm_counters/%s", name );
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

plugin_devops_t *pwr_xtpmdev_init( const char *initstr )
{
    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    *dev = devops;

    dev->private_data = malloc( sizeof(pwr_xtpmdev_t) );
    bzero( dev->private_data, sizeof(pwr_xtpmdev_t) );

    DBGP( "Info: initializing PWR XTPM device initstr=`%s`\n", initstr );

    return dev;
}

int pwr_xtpmdev_final( plugin_devops_t *dev )
{
    DBGP( "Info: finalizing PWR XTPM device\n" );

    free( dev->private_data );
    free( dev );
    return 0;
}

pwr_fd_t pwr_xtpmdev_open( plugin_devops_t *dev, const char *openstr )
{
    pwr_fd_t *fd = malloc( sizeof(pwr_xtpmfd_t) );
    bzero( fd, sizeof(pwr_xtpmfd_t) );

    DBGP( "Info: opening PWR XTPM device openstr=`%s`\n", openstr );

    PWR_XTPMFD(fd)->dev = PWR_XTPMDEV(dev->private_data);

    if( xtpmdev_read( "generation", &(PWR_XTPMFD(fd)->generation) ) < 0 ) {
        fprintf( stderr, "Error: unable to open generation counter\n" );
#if 1
        PWR_XTPMFD(fd)->dev = NULL;	
#else
        return 0x0;
#endif
    }

    return fd;
}

int pwr_xtpmdev_close( pwr_fd_t fd )
{
    DBGP( "Info: closing PWR XTPM device\n" );

    PWR_XTPMFD(fd)->dev = 0x0;
    free( fd );

    return 0;
}

int pwr_xtpmdev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    double generation;
    struct timeval tv;

    DBGP( "Info: reading from PWR XTPM device\n" );

    if ( NULL == PWR_XTPMFD(fd)->dev ) {
		*((double*)value) = 0;
		return 0;
	}	

    if( len != sizeof(double) ) {
        fprintf( stderr, "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(double) );
        return -1;
    }

    switch( attr ) {
        case PWR_ATTR_ENERGY:
            if( xtpmdev_read( "energy", (double *)value) < 0 ) {
                fprintf( stderr, "Error: unable to read energy counter\n" );
                return -1;
            }
            break;
        case PWR_ATTR_POWER:
            if( xtpmdev_read( "power", (double *)value) < 0 ) {
                fprintf( stderr, "Error: unable to read power counter\n" );
                return -1;
            }
            break;
        case PWR_ATTR_POWER_LIMIT_MAX:
            if( xtpmdev_read( "power_cap", (double *)value) < 0 ) {
                fprintf( stderr, "Error: unable to read power_cap counter\n" );
                return -1;
            }
            break;
        default:
            fprintf( stderr, "Warning: unknown PWR XTPM reading attr (%u) requested\n", attr );
            break;
    }
    gettimeofday( &tv, NULL );
    *timestamp = tv.tv_sec*1000000000ULL + tv.tv_usec*1000;

    if( xtpmdev_read( "generation", &generation) < 0 ) {
        fprintf( stderr, "Error: unable to open generation counter\n" );
        return 0x0;
    }
    if( PWR_XTPMFD(fd)->generation != generation ) {
        fprintf( stderr, "Warning: generation counter rolled over" );
        PWR_XTPMFD(fd)->generation = generation;
    }

    DBGP( "Info: reading of type %u at time %llu with value %lf\n",
        attr, *(unsigned long long *)timestamp, *(double *)value );
    return 0;
}

int pwr_xtpmdev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    DBGP( "Info: writing to PWR XTPM device\n" );

    if ( NULL == PWR_XTPMFD(fd)->dev ) {
		return 0;
	}

    if( len != sizeof(double) ) {
        fprintf( stderr, "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(double) );
        return -1;
    }

    switch( attr ) {
        case PWR_ATTR_POWER_LIMIT_MAX:
            if( xtpmdev_write( "power_cap", *((double *)value) ) < 0 ) {
                fprintf( stderr, "Error: unable to write power_cap counter\n" );
                return -1;
            }
            break;
        default:
            fprintf( stderr, "Warning: unknown PWR XTPM writing attr (%u) requested\n", attr );
            break;
    }

    DBGP( "Info: reading of type %u with value %lf\n", attr, *(double *)value );
    return 0;
}

int pwr_xtpmdev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_xtpmdev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );

    return 0;
}

int pwr_xtpmdev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_xtpmdev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

    return 0;
}

int pwr_xtpmdev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;

    DBGP( "Info: reading time from PWR XTPM device\n" );

    return pwr_xtpmdev_read( fd, PWR_ATTR_ENERGY, &value, sizeof(double), timestamp );;
}

int pwr_xtpmdev_clear( pwr_fd_t fd )
{
    return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_xtpmdev_init,
    .final  = pwr_xtpmdev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

//
// Start of plugin meta data
//

// Any thing can co here as long as it does not clash with other plugins
static int pwr_xtpm_getPluginName( size_t len, char* buf )
{
    strncpy(buf,"XTPM",len);
    return 0;
}

// What objects does this plugin support?
static int pwr_xtpm_readObjs(  int i, PWR_ObjType* ptr )
{
    DBGP("\n");
    ptr[0] = PWR_OBJ_NODE;
    return 0;
}

// How may object types does this plugin support?
// this is the size of the array referenced by pwr_xtpm_readObjs()
static int pwr_xtpm_numObjs( )
{
    DBGP("\n");
    return 1;
}

// What attributes does an object support?
// Note that currently this function does not specify Object type
// so all objects must support all attributues
// Yes, this is limiting.
// This interface could be revised to remove this restriction by adding
// PWR_ObjType as a parameter.
static int pwr_xtpm_readAttrs( int i, PWR_AttrName* ptr )
{
    DBGP("\n");
    ptr[0] = PWR_ATTR_ENERGY;
    ptr[1] = PWR_ATTR_POWER;
    ptr[2] = PWR_ATTR_POWER_LIMIT_MAX;
    return 0;
}

// How many attributes does an object support?
// this is the size of the array referenced by pwr_xtpm_readAttr()
static int pwr_xtpm_numAttrs( PWR_ObjType type )
{
    DBGP("\n");
    return 3;
}

// a plugin device can be initialized multiple times, once for each
// object type, however this is not necessry, you can have one device
// to handle all object types
static int pwr_xtpm_getDevName(PWR_ObjType type, size_t len, char* buf )
{
    strncpy(buf,"pwr_xtpm_dev0", len );
    DBGP("type=%d name=`%s`\n",type,buf);
    return 0;
}

// Create the device initialized string for the specified dev. The name
// was returned the the framework by pwr_xtpm_getDevName()
static int pwr_xtpm_getDevInitStr( const char* devName,
                        size_t len, char* buf )
{
    strncpy(buf,"",len);
    DBGP("dev=`%s` str=`%s`\n",devName, buf );
    return 0;
}

// a device can be opened multiple times, get the info to pass to the
// open call for this object type
static int pwr_xtpm_getDevOpenStr(PWR_ObjType type,
                        int global_index, size_t len, char* buf )
{
    snprintf( buf, len, "%d %d", type, global_index);
    DBGP("type=%d global_index=%d str=`%s`\n",type,global_index,buf);
    return 0;
}

static plugin_meta_t meta = {
    .numObjs = pwr_xtpm_numObjs,
    .numAttrs = pwr_xtpm_numAttrs,
    .readObjs = pwr_xtpm_readObjs,
    .readAttrs = pwr_xtpm_readAttrs,
    .getDevName = pwr_xtpm_getDevName,
    .getDevOpenStr = pwr_xtpm_getDevOpenStr,
    .getDevInitStr = pwr_xtpm_getDevInitStr,
    .getPluginName = pwr_xtpm_getPluginName,
};

plugin_meta_t* getMeta() {
    return &meta;
}
