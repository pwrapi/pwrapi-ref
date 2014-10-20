#include "pwr_cpudev.h"
#include "pwr_dev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

static int cpudev_verbose = 0;

typedef struct {
    int online_cpulist[1000];
    int freq_cpulist[1000];
    int num_cpus;
    int avail_freqlist[100];
    int num_freq;
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
    .stat_get     = pwr_dev_stat_get,
    .stat_start   = pwr_dev_stat_start,
    .stat_stop    = pwr_dev_stat_stop,
    .stat_clear   = pwr_dev_stat_clear,
    .private_data = 0x0
};

static int online_cpu( int cpu, int state )
{
    int fd;
    char onoff;
    char cpupath[100] = "";

    sprintf( cpupath, "/sys/devices/system/cpu/cpu%i/online", cpu );
    fd = open( cpupath, O_WRONLY );

    onoff = (state ? '1' : '0');
    write( fd, &onoff, 1 );

    close( fd );

    return 0;
}

static int change_freq( int cpu, int freq )
{
    int fd;
    char val[20] = "";
    char freqpath[100] = "";

    sprintf( freqpath, "/sys/devices/system/cpu/cpu%i/cpufreq", cpu );
    fd = open( freqpath, O_WRONLY );

    sprintf( val, "%d", freq );
    write( fd, val, strlen(val) );

    close( fd );

    return 0;
}

static int avail_freq( int cpu, int freq[], int *count )
{
    int fd;
    char freqpath[100] = "";
    char val[20] = "";
    int offset = 0;

    sprintf( freqpath, "/sys/devices/system/cpu/cpu%i/cpufreq/scaling_available_frequencies", cpu );
    fd = open( freqpath, O_RDONLY );

    while( read( fd, val+offset, 1 ) != EOF ) {
        if( val[offset] == ' ' ) {
            freq[*count] = atoi(val);
            (*count)++;
            bzero( val, strlen(val) );
            offset = 0;
        } else
            offset++;
    }
        
    close( fd );

    return 0;
}

plugin_devops_t *pwr_cpudev_init( const char *initstr )
{
    int i;

    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    *dev = devops;

    dev->private_data = malloc( sizeof(pwr_cpudev_t) );
    bzero( dev->private_data, sizeof(pwr_cpudev_t) );

    if( cpudev_verbose )
        printf( "Info: initializing PWR CPU device\n" );

    PWR_CPUDEV(dev->private_data)->num_cpus = sysconf(_SC_NPROCESSORS_CONF);
    for( i = 1; i <= PWR_CPUDEV(dev->private_data)->num_cpus -1; i++ ) {
        online_cpu( i, 1 );
        PWR_CPUDEV(dev->private_data)->online_cpulist[i] = 1;
    }        

    avail_freq(0, PWR_CPUDEV(dev->private_data)->avail_freqlist, &(PWR_CPUDEV(dev->private_data)->num_freq));

    return dev;
}

int pwr_cpudev_final( plugin_devops_t *dev )
{
    if( cpudev_verbose )
        printf( "Info: finaling PWR CPU device\n" );

    free( dev->private_data );
    free( dev );
    return 0;
}

pwr_fd_t pwr_cpudev_open( plugin_devops_t *dev, const char *openstr )
{
    char *token;

    pwr_fd_t *fd = malloc( sizeof(pwr_cpufd_t) );
    bzero( fd, sizeof(pwr_cpufd_t) );

    if( cpudev_verbose )
        printf( "Info: opening PWR CPU descriptor\n" );

    PWR_CPUFD(fd)->dev = PWR_CPUDEV(dev->private_data);

    if( openstr == 0x0 || (token = strtok( (char *)openstr, ":" )) == 0x0 ) {
        printf( "Error: missing CPU separator in initialization string %s\n", openstr );
        return 0x0;
    }
    PWR_CPUFD(fd)->cpu = atoi(token);

    if( cpudev_verbose )
        printf( "Info: extracted initialization string (CPU=%u)\n", PWR_CPUFD(fd)->cpu );

    return fd;
}

int pwr_cpudev_close( pwr_fd_t fd )
{
    if( cpudev_verbose )
        printf( "Info: closing PWR CPU descriptor\n" );

    PWR_CPUFD(fd)->dev = 0x0;
    free( fd );

    return 0;
}

int pwr_cpudev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    if( len != sizeof(unsigned long long) ) {
        printf( "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(unsigned long long) );
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
            *((unsigned long long *)value) =
                (unsigned long long)((PWR_CPUFD(fd)->dev)->online_cpulist[PWR_CPUFD(fd)->cpu]);
            break;
        case PWR_ATTR_FREQ:
            *((double *)value) = (double)0;
            break;
        case PWR_ATTR_TEMP:
            *((double *)value) = (double)0;
            break;
        default:
            printf( "Warning: unknown PWR reading attr (%u) requested\n", attr );
            break;
    }
    *timestamp = 0;

    if( cpudev_verbose )
        printf( "Info: reading of type %u at time %llu with value %lf\n",
                attr, *(unsigned long long *)timestamp, *(double *)value );

    return 0;
}

int pwr_cpudev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    if( len != sizeof(unsigned long long) ) {
        printf( "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(unsigned long long) );
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
            if( online_cpu( PWR_CPUFD(fd)->cpu, *((unsigned long long *)value) ) == 0 )
                (PWR_CPUFD(fd)->dev)->online_cpulist[PWR_CPUFD(fd)->cpu] =
                    *((unsigned long long *)value);
            break;
        case PWR_ATTR_FREQ:
            *((double *)value) = (double)0;
            break;
        case PWR_ATTR_TEMP:
            *((double *)value) = (double)0;
            break;
        default:
            printf( "Warning: unknown PWR writing attr (%u) requested\n", attr );
            break;
    }

    if( cpudev_verbose )
        printf( "Info: writing of type %u with value %lf\n", attr, *(double *)value );

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
    return 0;
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
