#include <assert.h>
#include "pwr_rapldev.h"
#include "pwr_dev.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <msr_core.h>
#include <msr_rapl.h>
#include "pwr_msrdev.h"
typedef struct {
	int fd;
	double energy;
} pwr_msrdev_t;
typedef struct {
	pwr_msrdev_t *dev;
} pwr_msrfd_t;
#define PWR_MSRFD(X) ((pwr_msrfd_t *)(X))


static plugin_devops_t devops = {
	.open         = pwr_msrdev_open,
	.close        = pwr_msrdev_close,
	.read         = pwr_msrdev_read,
	.write        = pwr_msrdev_write,
	.readv        = pwr_msrdev_readv,
	.writev       = pwr_msrdev_writev,
	.time         = pwr_msrdev_time,
	.clear        = pwr_msrdev_clear,
#if 0
	.stat_get     = pwr_dev_stat_get,
	.stat_start   = pwr_dev_stat_start,
	.stat_stop    = pwr_dev_stat_stop,
	.stat_clear   = pwr_dev_stat_clear,
#endif
	.private_data = 0x0
};


static struct rapl_data *rdat;
static uint64_t *rflags;
plugin_devops_t *pwr_msrdev_init( const char *initstr )
{
	//printf("msrdev_init\n");
	int i;

	plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );

	init_msr();
	rapl_init(&rdat, &rflags);
	//printf("msr inited\n");

	//DBGP("num packages %d\n", priv->numPkgs );
	//assert( priv->numPkgs > 0 );

	*dev = devops;
	//printf("set equaled to devops\n");
	dev->private_data = malloc( sizeof(pwr_msrdev_t) );
	//printf("malloced\n");
	bzero( dev->private_data, sizeof(pwr_msrdev_t) );
	//printf("init finished\n");
	return dev;
}

int pwr_msrdev_final( plugin_devops_t *dev )
{
	free( dev->private_data );
	free( dev );

	return 0;
}
pwr_fd_t pwr_msrdev_open( plugin_devops_t *dev, const char *openstr )
{
	//printf("msr opened\n");
	pwr_msrfd_t *fd = malloc( sizeof(pwr_msrfd_t) );
	PWR_MSRFD(fd)->dev =(pwr_msrdev_t *)(dev->private_data);
	PWR_MSRFD(fd)->dev->energy = 0.0;
	return fd;
}
int pwr_msrdev_close( pwr_fd_t fd )
{
	PWR_MSRFD(fd)->dev = 0x0;
	free( fd );

	return 0;
}

int pwr_msrdev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
	int i;
	double energy = 0.0;
	double time = 0.0;
	int policy = 0;
#ifndef USE_SYSTIME
	struct timeval tv;
#endif
	energy=PWR_MSRFD(fd)->dev->energy;

	if( len != sizeof(double) ) {
		fprintf( stderr, "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(double) );
		return -1;
	}
	poll_rapl_data();
	int snum = num_sockets();
	for(int s = 0; s<snum; s++)
	{
		energy = rdat->pkg_delta_joules[s];
	}
	double total;
	switch( attr ) {
		case PWR_ATTR_ENERGY:
			*((double *)value) = energy;
			break;
		case PWR_ATTR_POWER_LIMIT_MAX:
			total = 0;
			for(int s = 0; s<snum; s++)
			{
				struct rapl_limit rlimg;
	        	        get_pkg_rapl_limit(s, &rlimg, NULL);
				total+=rlimg.watts;
			}
			*((double *)value) = total;
			break;

		default:
			fprintf( stderr, "Warning: unknown PWR reading attr requested\n" );
			break;
	}

	PWR_MSRFD(fd)->dev->energy=energy;
#ifndef USE_SYSTIME
	gettimeofday( &tv, NULL );
	*timestamp = tv.tv_sec*1000000000ULL + tv.tv_usec*1000;
#else
	*timestamp = (unsigned int)time*1000000000ULL + 
		(time-(unsigned int)time)*1000000000ULL;
#endif

	return 0;
}
int pwr_msrdev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{

	struct rapl_limit rlim;
	struct rapl_limit rlim2;
	int snum;
	double val = *(double *)value;
	poll_rapl_data();
	switch( attr )
	{
		case PWR_ATTR_POWER_LIMIT_MAX:
			snum = num_sockets();
			rlim.watts=val/snum;
			rlim2.watts=val/snum;
			rlim.bits=0;
			rlim2.bits=0;
			rlim.seconds=1;
			rlim2.seconds=1;
			for(int s = 0; s<snum; s++)
			{
						set_pkg_rapl_limit(s, &rlim, &rlim2);
			}
			break;
		default:
			fprintf(stderr,"Warning: unknown PWR writing attr requested\n");
			break;
	}
	return 0;
}

int pwr_msrdev_readv( pwr_fd_t fd, unsigned int arraysize,
		const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
	unsigned int i;

	for( i = 0; i < arraysize; i++ )
		status[i] = pwr_msrdev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );

	return 0;
}

int pwr_msrdev_writev( pwr_fd_t fd, unsigned int arraysize,
		const PWR_AttrName attrs[], void *values, int status[] )
{
	unsigned int i;

	for( i = 0; i < arraysize; i++ )
		status[i] = pwr_msrdev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

	return 0;
}

int pwr_msrdev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
	double value;

	DBGP( "Info: PWR MSR device time\n" );

	return pwr_msrdev_read( fd, PWR_ATTR_POWER, &value, sizeof(double), timestamp );
}

int pwr_msrdev_clear( pwr_fd_t fd )
{
	return 0;
}

static plugin_dev_t dev = {
	.init   = pwr_msrdev_init,
	.final  = pwr_msrdev_final,
};

plugin_dev_t* getDev() {
	return &dev;
}

static int msrdev_numObjs( )
{
	DBGP("\n");
	return 1;
}
static int msrdev_readObjs(  int i, PWR_ObjType* ptr )
{
	DBGP("\n");
	ptr[0] = PWR_OBJ_NODE;
	return 0;
}

static int msrdev_numAttrs( )
{
	DBGP("\n");
	return 2;
}

static int msrdev_readAttrs( int i, PWR_AttrName* ptr )
{
	DBGP("\n");
	ptr[0] = PWR_ATTR_ENERGY;
	ptr[1] = PWR_ATTR_POWER_LIMIT_MAX;
	return 0;
}

static int msrdev_getDevName(PWR_ObjType type, size_t len, char* buf )
{
	strncpy(buf,"pwr_msr_dev0", len );
	DBGP("type=%d name=`%s`\n",type,buf);
	return 0;
}

static int msrdev_getDevOpenStr(PWR_ObjType type,
		int global_index, size_t len, char* buf )
{
	snprintf( buf, len, "%d %d", type, global_index);
	DBGP("type=%d global_index=%d str=`%s`\n",type,global_index,buf);
	return 0;
}

static int msrdev_getDevInitStr( const char* name,
		size_t len, char* buf )
{
	snprintf(buf,len,"");
	DBGP("dev=`%s` str=`%s`\n",name, buf );
	return 0;
}

static int msrdev_getPluginName( size_t len, char* buf )
{
	snprintf(buf,len,"MSR");
	return 0;
}

static plugin_meta_t meta = {
	.numObjs = msrdev_numObjs,
	.numAttrs = msrdev_numAttrs,
	.readObjs = msrdev_readObjs,
	.readAttrs = msrdev_readAttrs,
	.getDevName = msrdev_getDevName,
	.getDevOpenStr = msrdev_getDevOpenStr,
	.getDevInitStr = msrdev_getDevInitStr,
	.getPluginName = msrdev_getPluginName,
};

plugin_meta_t* getMeta() {
	return &meta;
}

