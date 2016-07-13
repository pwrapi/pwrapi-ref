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

#include <assert.h>
#include "pwr_cpudev.h"
#include "pwr_dev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

typedef struct {
    int num_cpus;
    int num_freq;
    int avail_freqlist[100];
} pwr_cpudev_t;
#define PWR_CPUDEV(X) ((pwr_cpudev_t *)(X))

typedef struct {
    pwr_cpudev_t *dev;
    int cpu;
} pwr_cpufd_t;
#define PWR_CPUFD(X) ((pwr_cpufd_t *)(X))

static plugin_devops_t devops = {
    .open         = pwr_cpudev_open,
    .close        = pwr_cpudev_close,
    .read         = pwr_cpudev_read,
    .write        = pwr_cpudev_write,
    .readv        = pwr_cpudev_readv,
    .writev       = pwr_cpudev_writev,
    .time         = pwr_cpudev_time,
    .clear        = pwr_cpudev_clear,
#if 0
    .stat_get     = pwr_dev_stat_get,
    .stat_start   = pwr_dev_stat_start,
    .stat_stop    = pwr_dev_stat_stop,
    .stat_clear   = pwr_dev_stat_clear,
#endif
    .private_data = 0x0
};

static int cpudev_read( int cpu, const char *name, double *val )
{
    char path[256] = "", strval[20] = "";
    int offset = 0;
    int fd;

    sprintf( path, "/sys/devices/system/cpu/cpu%i/%s", cpu, name );
	DBGP("%s\n",path);
    fd = open( path, O_RDONLY );
    if( fd < 0 ) {
        fprintf( stderr, "Error: unable to open CPU file at %s\n", path );
        return -1;
    }

    while( read( fd, strval+offset, 1 ) != EOF ) {
        if( strval[offset] == ' ' ) {
            *val = atof(strval);
            return 0;
        }
        offset++;
    }

    fprintf( stderr, "Error: unable to parse PM counter value\n" );
    return -1;
}

static int cpudev_write( int cpu, const char *name, double val )
{
    char path[256] = "", strval[20] = "";
    int fd;

    sprintf( path, "/sys/devices/system/cpu/cpu%i/%s", cpu, name );
    fd = open( path, O_WRONLY );
    if( fd < 0 ) {
        fprintf( stderr, "Error: unable to open CPU file at %s\n", path );
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

static int cpudev_avail_freq( int cpu, int freq[], int *count )
{
    char freqpath[100] = "";
    char val[20] = "";
    int offset = 0;
	int c;

    sprintf( freqpath, "/sys/devices/system/cpu/cpu%i/cpufreq/scaling_available_frequencies", cpu );

    DBGP( "%s\n", freqpath );

	FILE* fp = fopen( freqpath, "r" );
	if ( ! fp ) {
		fprintf(stderr,"Error: plugin 'cpudev' can't open `%s`\n",freqpath);
		assert(0);
	}

	while ( EOF != ( c = fgetc( fp ) ) ) {
		if ( isdigit( c ) ) { 
			val[offset++] = c;
		} else if ( c == ' ' ) {
			assert( offset != 0 );
            freq[*count] = atoi(val);
			DBGP("add freq %u\n",freq[(*count)++]);
			offset = 0;	
		}
	} 
	fclose(fp);

    return 0;
}

plugin_devops_t *pwr_cpudev_init( const char *initstr )
{
    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    *dev = devops;

    dev->private_data = malloc( sizeof(pwr_cpudev_t) );
    bzero( dev->private_data, sizeof(pwr_cpudev_t) );

    DBGP( "Info: initializing PWR CPU device\n" );

    PWR_CPUDEV(dev->private_data)->num_cpus = sysconf(_SC_NPROCESSORS_CONF);
    cpudev_avail_freq(0, PWR_CPUDEV(dev->private_data)->avail_freqlist, &(PWR_CPUDEV(dev->private_data)->num_freq));

    DBGP( "\n" );
    return dev;
}

int pwr_cpudev_final( plugin_devops_t *dev )
{
    DBGP( "Info: finaling PWR CPU device\n" );

    free( dev->private_data );
    free( dev );
    return 0;
}

pwr_fd_t pwr_cpudev_open( plugin_devops_t *dev, const char *openstr )
{
    char *token;

    pwr_fd_t *fd = malloc( sizeof(pwr_cpufd_t) );
    bzero( fd, sizeof(pwr_cpufd_t) );

    DBGP( "Info: opening PWR CPU descriptor\n" );

    PWR_CPUFD(fd)->dev = PWR_CPUDEV(dev->private_data);

    if( openstr == 0x0 || (token = strtok( (char *)openstr, ":" )) == 0x0 ) {
        fprintf( stderr, "Error: missing CPU separator in initialization string %s\n", openstr );
        return 0x0;
    }
    PWR_CPUFD(fd)->cpu = atoi(token);

    DBGP( "Info: extracted initialization string (CPU=%u)\n", PWR_CPUFD(fd)->cpu );

    return fd;
}

int pwr_cpudev_close( pwr_fd_t fd )
{
    DBGP( "Info: closing PWR CPU descriptor\n" );

    PWR_CPUFD(fd)->dev = 0x0;
    free( fd );

    return 0;
}

int pwr_cpudev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    struct timeval tv;

    if( len != sizeof(unsigned long long) ) {
        fprintf( stderr, "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(unsigned long long) );
        return -1;
    }

    switch( attr ) {
        case PWR_ATTR_PSTATE:
            *((unsigned long long *)value) = (unsigned long long)0;
            break;
        case PWR_ATTR_CSTATE:
            *((unsigned long long *)value) = (unsigned long long)0;
            break;
        case PWR_ATTR_SSTATE:
            if( cpudev_read( PWR_CPUFD(fd)->cpu, "online", (double *)value) < 0 ) {
                fprintf( stderr, "Error: unable to read cpu %d sleep state\n", PWR_CPUFD(fd)->cpu );
                return -1;
            }
            break;
        case PWR_ATTR_FREQ:
            if( cpudev_read( PWR_CPUFD(fd)->cpu, "cpufreq", (double *)value) < 0 ) {
                fprintf( stderr, "Error: unable to read cpu %d frequency\n", PWR_CPUFD(fd)->cpu );
                return -1;
            }
            break;
        case PWR_ATTR_TEMP:
            *((double *)value) = (double)0;
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

int pwr_cpudev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    if( len != sizeof(unsigned long long) ) {
        fprintf( stderr, "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(unsigned long long) );
        return -1;
    }

    switch( attr ) {
        case PWR_ATTR_PSTATE:
            *((unsigned long long *)value) = (unsigned long long)0;
            break;
        case PWR_ATTR_CSTATE:
            *((unsigned long long *)value) = (unsigned long long)0;
            break;
        case PWR_ATTR_SSTATE:
            if( cpudev_write( PWR_CPUFD(fd)->cpu, "online", *((double *)value)) < 0 ) {
                fprintf( stderr, "Error: unable to write cpu %d sleep state\n", PWR_CPUFD(fd)->cpu );
                return -1;
            }
            break;
        case PWR_ATTR_FREQ:
            if( cpudev_write( PWR_CPUFD(fd)->cpu, "cpufreq", *((double *)value)) < 0 ) {
                fprintf( stderr, "Error: unable to write cpu %d frequency\n", PWR_CPUFD(fd)->cpu );
                return -1;
            }
            break;
        case PWR_ATTR_TEMP:
            *((double *)value) = (double)0;
            break;
        default:
            fprintf( stderr, "Warning: unknown PWR writing attr (%u) requested\n", attr );
            break;
    }

    DBGP( "Info: writing of type %u with value %lf\n", attr, *(double *)value );

    return 0;
}

int pwr_cpudev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_cpudev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );

    return 0;
}

int pwr_cpudev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_cpudev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

    return 0;
}

int pwr_cpudev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;

    DBGP( "Info: reading time from CPU device\n" );

    return pwr_cpudev_read( fd, PWR_ATTR_FREQ, &value, sizeof(double), timestamp );;
}

int pwr_cpudev_clear( pwr_fd_t fd )
{
    return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_cpudev_init,
    .final  = pwr_cpudev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}
static int pwr_cpudev_numObjs( )
{
    DBGP("\n");
    return 1;
}
static int pwr_cpudev_readObjs(  int i, PWR_ObjType* ptr )
{
    DBGP("\n");
    ptr[0] = PWR_OBJ_CORE;
	return 0;
}

static int pwr_cpudev_numAttrs( )
{
    DBGP("\n");
    return 2;
}

static int pwr_cpudev_readAttrs( int i, PWR_AttrName* ptr )
{
    DBGP("\n");
    ptr[0] = PWR_ATTR_SSTATE;
    ptr[1] = PWR_ATTR_FREQ;
// these are not supported
//    ptr[] = PWR_ATTR_PSTATE;
//    ptr[] = PWR_ATTR_CSTATE;
//    ptr[] = PWR_ATTR_TEMP;
    return 0;
}

static int pwr_cpudev_getDevName(PWR_ObjType type, size_t len, char* buf )
{
    strncpy(buf,"cpu_dev0", len );
    DBGP("type=%d name=`%s`\n",type,buf);
	return 0;
}

static int pwr_cpudev_getDevOpenStr(PWR_ObjType type,
                        int global_index, size_t len, char* buf )
{
    snprintf( buf, len, "%d", global_index);
    DBGP("type=%d global_index=%d str=`%s`\n",type,global_index,buf);
	return 0;
}

static int pwr_cpudev_getDevInitStr( const char* name,
                        size_t len, char* buf )
{
    strncpy(buf,"",len);
    DBGP("dev=`%s` str=`%s`\n",name, buf );
	return 0;
}

static int pwr_cpudev_getPluginName( size_t len, char* buf )
{
    strncpy(buf,"CPU",len);
	return 0;
}

static plugin_meta_t meta = {
    .numObjs = pwr_cpudev_numObjs,
    .numAttrs = pwr_cpudev_numAttrs,
    .readObjs = pwr_cpudev_readObjs,
    .readAttrs = pwr_cpudev_readAttrs,
    .getDevName = pwr_cpudev_getDevName,
    .getDevOpenStr = pwr_cpudev_getDevOpenStr,
    .getDevInitStr = pwr_cpudev_getDevInitStr,
    .getPluginName = pwr_cpudev_getPluginName,
};

plugin_meta_t* getMeta() {
    return &meta;
}
