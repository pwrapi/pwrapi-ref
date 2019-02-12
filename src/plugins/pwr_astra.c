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
#define _GNU_SOURCE
#include <sys/timeb.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <errno.h>

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include <stdint.h>
#include <mc_oper_region.h>

#include "pwr_dev.h"
#include "util.h"

struct pwr_socmonFdInfo;

#define PATH_T99MON_DEV     "/sys/bus/platform/devices/tx2mon"
#define PATH_T99MON_NODE0   PATH_T99MON_DEV "/node0_raw"
#define PATH_T99MON_NODE1   PATH_T99MON_DEV "/node1_raw"
#define PATH_T99MON_SOCINFO PATH_T99MON_DEV "/socinfo"

static void fatal( const char* msg ) {
	fprintf(stderr, "Fatal: %s\n",msg );
	exit(1);
}

static inline double to_v(int mv)
{
    return mv/1000.0;
}

static inline double to_w(int mw)
{
    return mw/1000.0;
}

struct node_data {
    int fd;
    int cores;
    int node;
    struct  mc_oper_region buf;
};

struct tx2mon {
    int     nodes;
    int     samples;
    FILE        *fileout;
    volatile int    stop;
    struct  node_data node[2];
};


typedef struct {
	struct tx2mon tx2mon;
	pthread_t thread;
	useconds_t sleep_us;
	int done;
	struct pwr_socmonFdInfo** energyObjs;
	double lastSample;

	FILE** scaling_setspeed_fd;

} pwr_socmonDevInfo_t;


typedef struct pwr_socmonFdInfo {
	PWR_ObjType objType;
	int id;
	pwr_socmonDevInfo_t* devInfo;
	double energy;
} pwr_socmonFdInfo_t;

static int readDev( pwr_socmonFdInfo_t*, PWR_ObjType, int id, PWR_AttrName name, void *value, unsigned int len, PWR_Time *time );
static int writeDev( pwr_socmonFdInfo_t*, PWR_ObjType, int id, PWR_AttrName name, void *value, unsigned int len );

static double getTime() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	double value; 
	value = tv.tv_sec * 1000000000;
	value += tv.tv_usec * 1000;
	return value;
}

static double getTimeSec()
{
	return getTime() / 1000000000.0;
}
  
static pwr_fd_t pwr_socmon_dev_open( plugin_devops_t* ops, const char *openstr )
{
	DBGP("openstr=`%s`\n",openstr);
	pwr_socmonFdInfo_t *tmp = malloc( sizeof( pwr_socmonFdInfo_t ) );
	tmp->devInfo = ops->private_data;
	tmp->energy = 0;

	sscanf(openstr, "%d %d", &tmp->objType, &tmp->id );
	DBGP("open obj=%s id=%d\n", objTypeToString(tmp->objType), tmp->id);

	if ( tmp->objType == PWR_OBJ_SOCKET ) {
		DBGP("calculate energy\n");
		tmp->devInfo->energyObjs[tmp->id] = tmp;
	}

	return tmp;
}

static int pwr_socmon_dev_close( pwr_fd_t fd )
{
	pwr_socmonFdInfo_t* info = fd; 
	if ( info->objType == PWR_OBJ_SOCKET ) {
		DBGP("calculate energy\n");
		info->devInfo->energyObjs[info->id] = NULL;
	}
    free( fd );
    return 0;
}

static int pwr_socmon_dev_read( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len, PWR_Time* ts )
{
	pwr_socmonFdInfo_t* info = fd;
	return readDev( fd, info->objType, info->id, type, ptr, len, ts );
}

static int pwr_socmon_dev_write( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len )
{
	pwr_socmonFdInfo_t* info = fd;
	return writeDev( fd, info->objType, info->id, type, ptr, len );
}

static plugin_devops_t devOps = {
	.open   = pwr_socmon_dev_open, 
	.close  = pwr_socmon_dev_close,
	.read   = pwr_socmon_dev_read,
	.write  = pwr_socmon_dev_write,
	.readv  = NULL,
	.writev = NULL,
	.time   = NULL,
	.clear  = NULL,
	.log_start = NULL,
	.log_stop = NULL,
	.get_samples = NULL,
};

static pwr_socmonDevInfo_t* socmonInit( const char* devName);
static void socmonFini( pwr_socmonDevInfo_t* devInfo );

#define SYSFS_CPU "/sys/devices/system/cpu"

static plugin_devops_t* pwr_socmon_dev_init( const char *initstr )
{
	pwr_socmonDevInfo_t* devInfo = socmonInit( initstr );

	if ( NULL == devInfo ) {
		return NULL;
	}
	
	devInfo->scaling_setspeed_fd = NULL;

#if 0
	devInfo->scaling_setspeed_fd = malloc( sizeof( FILE* ) * devInfo->nnodes * devInfo->cpus_per_soc );
	int i;
	for ( i = 0; i < devInfo->cpus_per_soc*devInfo->nnodes; i++ ) {
		char filename[PATH_MAX];
		sprintf( filename, "%s/cpu%d/cpufreq/scaling_governor",SYSFS_CPU,i);
		DBGP("%s\n",filename);
		FILE* fp = fopen( filename, "w+" ); 
		if ( NULL == fp ) {	
			char errorBuf[1024];
			fprintf(stderr,"fopen( \"%s\", \"w\" ) failed: %s \n",filename, strerror_r(errno,errorBuf,1024) );
			return NULL;
		}
		char buf[ 1024];
		char* str = "userspace";
		
		fprintf( fp, "%s", str );
		rewind(fp);
		fscanf( fp, "%s", buf );
		
		if ( 0 != strcmp( str, buf ) ) {
			fprintf(stderr," could not set \"%s\" to \"%s\"\n", filename, str );
			return NULL;
		}
		fclose(fp);

		sprintf( filename, "%s/cpu%d/cpufreq/scaling_setspeed",SYSFS_CPU,i);
		DBGP("%s\n",filename);
		fp = fopen( filename, "w+" ); 
		if ( NULL == fp ) {	
			char errorBuf[1024];
			fprintf(stderr,"fopen( \"%s\", \"w\" ) failed: %s \n",filename, strerror_r(errno,errorBuf,1024) );
			return NULL;
		}

		fscanf( fp, "%s", buf );
		devInfo->scaling_setspeed_fd[i] = fp; 
	}
#endif

	plugin_devops_t* ops = malloc(sizeof(*ops));
	*ops = devOps;

	ops->private_data = devInfo;

	return ops;
}

static int pwr_socmon_dev_final( plugin_devops_t *ops )
{
	pwr_socmonDevInfo_t* devInfo = ops->private_data;
	DBGP("\n");
	devInfo->done = 1;
	pthread_join( devInfo->thread, NULL ); 
	socmonFini( devInfo ); 
	free( ops );
	return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_socmon_dev_init, 
    .final  = pwr_socmon_dev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

//
// Start of plugin meta data
//

// Any thing can co here as long as it does not clash with other plugins
static int pwr_socmon_getPluginName( size_t len, char* buf )
{
    strncpy(buf,"Astra",len);
	return 0;
}

struct MetaObjectInfo {
	int numAttrs;
	PWR_AttrName attrs[PWR_NUM_ATTR_NAMES];
};

struct MetaInfo {
	int    num_objects;
	struct MetaObjectInfo metaObj[PWR_NUM_OBJ_TYPES];// = {NULL,};
};


static struct MetaInfo _metaInfo;

int addAttr( struct MetaObjectInfo* info, PWR_AttrName attr ) {
	info->attrs[info->numAttrs++] = attr;
}

static void initMeta()
{
	int i;
	for ( i=0; i < PWR_NUM_OBJ_TYPES; i++ ) {
		_metaInfo.metaObj[i].numAttrs = 0; 
	}
	_metaInfo.num_objects = 3;

	addAttr( &_metaInfo.metaObj[PWR_OBJ_NODE], PWR_ATTR_POWER );

	addAttr( &_metaInfo.metaObj[PWR_OBJ_CORE], PWR_ATTR_FREQ );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_CORE], PWR_ATTR_TEMP );

	addAttr( &_metaInfo.metaObj[PWR_OBJ_SOCKET], PWR_ATTR_TEMP );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_SOCKET], PWR_ATTR_POWER );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_SOCKET], PWR_ATTR_VOLTAGE );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_SOCKET], PWR_ATTR_FREQ );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_SOCKET], PWR_ATTR_ENERGY );
}

// What objects does this plugin support?
static int pwr_socmon_readObjs(  int numReq, PWR_ObjType* ptr )
{
	assert( numReq == _metaInfo.num_objects );
	int i;
	int pos = 0;

	for ( i = 0; i < PWR_NUM_OBJ_TYPES; i++ ) {
		if ( _metaInfo.metaObj[i].numAttrs ) {
			DBGP("have %s\n",objTypeToString(i));
			ptr[pos++] = i;
		}
	} 
	return 0;
}

// How may object types does this plugin support?
// this is the size of the array referenced by pwr_socmon_readObjs() 
static int pwr_socmon_numObjs( )
{
	DBGP("numObjs=%d\n",_metaInfo.num_objects);
	return _metaInfo.num_objects;
} 

// What attributes does an object support?
// Note that currently this function does not specify Object type
// so all objects must support all attributues
// Yes, this is limiting. 
// This interface could be revised to remove this restriction by adding
// PWR_ObjType as a parameter.
static int pwr_socmon_readAttrs( PWR_ObjType type, int numReq, PWR_AttrName* ptr )
{
	assert( numReq == _metaInfo.metaObj[type].numAttrs ); 
	int i;
	int pos = 0;	

	for ( i = 0; i < _metaInfo.metaObj[type].numAttrs; i++) {
		DBGP("%s has %s\n", objTypeToString(type), attrNameToString(_metaInfo.metaObj[type].attrs[i]));
		ptr[pos++] = _metaInfo.metaObj[type].attrs[i];
	}

	return 0;
}

// How many attributes does an object support?
// this is the size of the array referenced by pwr_socmon_readAttr()
static int pwr_socmon_numAttrs( PWR_ObjType type )
{
	DBGP("%s has %d attributes\n", objTypeToString(type), _metaInfo.metaObj[type].numAttrs);
	return _metaInfo.metaObj[type].numAttrs;
}

// a plugin device can be initialized multiple times, once for each
// object type, however this is not necessry, you can have one device
// to handle all object types
static int pwr_socmon_getDevName(PWR_ObjType type, size_t len, char* buf )
{
	strncpy(buf,"pwr_socmon_dev0", len );
	DBGP("type=%d name=`%s`\n",type,buf);
	return 0;
}

// Create the device initialized string for the specified dev. The name
// was returned the the framework by pwr_socmon_getDevName() 
static int pwr_socmon_getDevInitStr( const char* devName,
                        size_t len, char* buf )
{
	//strncpy(buf,"/dev/socmon",len);
	strncpy(buf,"",len);
	DBGP("dev=`%s` str=`%s`\n",devName, buf );
	return 0;
}

// a device can be opened multiple times, get the info to pass to the 
// open call for this object type 
static int pwr_socmon_getDevOpenStr(PWR_ObjType type,
                        int global_index, size_t len, char* buf )
{
 	snprintf( buf, len, "%d %d", type, global_index);
	DBGP("type=%d global_index=%d str=`%s`\n",type,global_index,buf);
	return 0;
}

static plugin_meta_t meta = {
	.numObjs = pwr_socmon_numObjs,
	.numAttrs = pwr_socmon_numAttrs,
	.readObjs = pwr_socmon_readObjs,
	.readAttrs = pwr_socmon_readAttrs,
	.getDevName = pwr_socmon_getDevName,
	.getDevOpenStr = pwr_socmon_getDevOpenStr,
	.getDevInitStr = pwr_socmon_getDevInitStr,
	.getPluginName = pwr_socmon_getPluginName,
};

plugin_meta_t* getMeta() {
	initMeta();
	return &meta;
}

static int inline getNode( pwr_socmonDevInfo_t* dev, int id ) {
	return id / dev->tx2mon.node[0].cores;
}
static int inline getCore( pwr_socmonDevInfo_t* dev, int id ) {
	return id % dev->tx2mon.node[0].cores;
}
static int inline getNumNodes( pwr_socmonDevInfo_t* dev ) {
	return dev->tx2mon.nodes;
}
static int inline getCpusPerNode( pwr_socmonDevInfo_t* dev ) {
	return dev->tx2mon.node[0].cores;
}

static void energy( pwr_socmonDevInfo_t* devInfo ) {
	struct timeval ts;
	gettimeofday( &ts, NULL );
	double now = getTimeSec();
	DBGP("time %f\n",now);	
	int i;
	for ( i = 0; i < getNumNodes( devInfo ); i++ ) {
		
		pwr_socmonFdInfo_t* fd = devInfo->energyObjs[i];
		if ( fd && devInfo->lastSample ) {
			double value;
			pwr_socmon_dev_read( fd, PWR_ATTR_POWER, &value, sizeof(value), NULL );
			fd->energy += (now - devInfo->lastSample ) * value;
			DBGP("POWER %f, ENERGY %f\n",value, fd->energy);	
		}
	} 
	devInfo->lastSample = now;
}

static void* thread( void* info ) {
	DBGP("\n");	
	pwr_socmonDevInfo_t* devInfo = info;
	while( ! devInfo->done ) {
		usleep( devInfo->sleep_us );
		energy( devInfo );
	}
}

static int initSampleThread( pwr_socmonDevInfo_t* devInfo );

static void init_socinfo(struct tx2mon* tx2mon )
{
    char *path;
    char buf[32];
    int fd, ret;
    int nodes, cores, threads;

    path = realpath(PATH_T99MON_SOCINFO, NULL);
    if (path == NULL) {
        fatal("couldn't create path to socinfo" );
    }

    fd = open(path, O_RDONLY);
    if (fd < 0)
        fatal("couldn't open socinfo" );

    ret = read(fd, buf, sizeof(buf));
    if (ret <= 0)
        fatal("couldn't read socinfo" );

    ret = sscanf(buf, "%d %d %d", &nodes, &cores, &threads);
    if (ret != 3)
        fatal("bad socinfo format" );

    close(fd);
    free(path);
    tx2mon->nodes = nodes;
    tx2mon->node[0].node = 0;
    tx2mon->node[0].cores = cores;
    if (nodes > 1) {
        tx2mon->node[1].node = 1;
        tx2mon->node[1].cores = cores;
		assert( tx2mon->node[0].cores == tx2mon->node[1].cores );
    }
}

static pwr_socmonDevInfo_t* socmonInit( const char* devName)
{
	int fd;
	int node_id;
	pwr_socmonDevInfo_t* devInfo = malloc( sizeof( pwr_socmonDevInfo_t ) );
	devInfo->lastSample = 0;

	init_socinfo( &devInfo->tx2mon );

	devInfo->energyObjs = malloc( sizeof( pwr_socmonFdInfo_t* ) * devInfo->tx2mon.nodes );
	bzero( devInfo->energyObjs, sizeof( pwr_socmonFdInfo_t* ) * devInfo->tx2mon.nodes );

    fd = open(PATH_T99MON_NODE0, O_RDONLY);
    if (fd < 0)
        fatal("Couldn't read tx2mon node0 entry");

    devInfo->tx2mon.node[0].fd = fd;

    if (devInfo->tx2mon.nodes > 1) {
        fd = open(PATH_T99MON_NODE1, O_RDONLY);
        if (fd < 0)
        	fatal("Couldn't read tx2mon node1 entry");

        devInfo->tx2mon.node[1].fd = fd;
    }

	if( initSampleThread( devInfo ) ) {
		free( devInfo );
		return NULL;
	}
	return devInfo; 
}

static int initSampleThread( pwr_socmonDevInfo_t* devInfo ) 
{
	devInfo->done = 0;
	
	devInfo->sleep_us  = 10000;
	char *envPtr;
	if ( envPtr = getenv( "PWR_SOCMON_POLL_US" ) ) {
		devInfo->sleep_us = atoi( envPtr );
	}
	envPtr = getenv( "PWR_SOCMON_THREAD_AFFINITY");

	pthread_create( &devInfo->thread, NULL, thread, devInfo );

	if ( envPtr ) {
		printf("thread affinity %s\n",envPtr);
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		int pos = 0;
		
		while ( envPtr[pos] ) {
			int cpu;
			int end = pos;
			while( envPtr[end] && envPtr[end] != ',' ) {
				++end;
			} 

			if ( envPtr[end] == ',' ) {
				envPtr[end] = 0;
				++end; 
			}

			cpu = atoi( envPtr + pos);	
			printf("add cpu %d to affinity mask\n",cpu);
			pos = end;
			CPU_SET(cpu,&cpuset);
		}

		if ( pthread_setaffinity_np( devInfo->thread,  sizeof(cpu_set_t), &cpuset )	!= 0 ) { 
			fatal("pthread_setaffinity_np failed" );
		}
	}
	return 0;
}

static void socmonFini( pwr_socmonDevInfo_t* devInfo ) {
	int i;
	free( devInfo );
}


static int read_node(struct node_data *d)
{
    int rv;
    struct mc_oper_region *op = &d->buf;

    rv = lseek(d->fd, 0, SEEK_SET);
    if (rv < 0)
           return rv;
    rv = read(d->fd, op, sizeof(*op));
    if (rv < sizeof(*op))
        return rv;
    if ((op->cmd_status & STATUS_READY) == 0)
        return 0;
    return 1;
}

static int readNode( pwr_socmonFdInfo_t* fd, int id, PWR_AttrName name, void *value, unsigned int len, PWR_Time *time ) {

	int ret = PWR_RET_FAILURE; 
	switch( name ) {
	case PWR_ATTR_POWER:
		{
#if 0
			char* cmd = "ipmitool sensor reading NodeDCpower";	
			char buf[512];
			int power;
			char foo[100];
			FILE* fp = popen( cmd, "r" );
			if ( NULL == fp ) {
				fatal("popen failed" );
			}

			if ( NULL ==  fgets( buf, 512, fp ) ) {
				fatal("ipmitool command failed ");
			}

			pclose(fp);

			sscanf(buf,"%s %s %d\n",foo, foo, &power);
			*(double*) value = power;
#else
			#define FILE_NAME "/tmp/NodePower"
			FILE* fp = fopen( FILE_NAME, "r" );
			if ( fp ) {
				
				float tmp;
				if ( 1 == fscanf( fp, "%f", &tmp ) ) {
					ret = PWR_RET_SUCCESS;
					*(double*) value = tmp; 
				}
				fclose(fp);
			}
#endif
		}
		break;
	default:
		assert(0);
	}

	return ret;
}

static int readCore( pwr_socmonFdInfo_t* fd, int id, PWR_AttrName name, void *value, unsigned int len, PWR_Time *time ) {

	pwr_socmonDevInfo_t* dev = fd->devInfo;

	int soc = getNode( dev, id );
	int core= getCore( dev, id );
	DBGP("coreId=%d attr=%s numSoc=%d numCores=%d soc=%d core=%d\n", id, attrNameToString(name), 
										getNumNodes( dev ), getCpusPerNode( dev ),soc,core);

	assert( sizeof(double) == len );
	assert( soc < getNumNodes( dev ) );
	assert( core < getCpusPerNode( dev ) );

	read_node(&dev->tx2mon.node[soc]);
	struct mc_oper_region* pCtrl = &dev->tx2mon.node[soc].buf;	

	switch( name ) {
	case PWR_ATTR_FREQ:
		*(double*) value = pCtrl->freq_cpu[core];
		break;
	case PWR_ATTR_TEMP:
		*(double*) value = to_c( pCtrl->tmon_cpu[core] );
		break;
	default:
		assert(0);
	}
    if ( time ) {
		*time = getTimeSec();
    }
    return PWR_RET_SUCCESS;
}

static int readSocket( pwr_socmonFdInfo_t* fd, int id, PWR_AttrName name, void *value, unsigned int len, PWR_Time *time ) {

	pwr_socmonDevInfo_t* dev = fd->devInfo;

	DBGP("socId=%d attr=%s numSoc=%d soc=%d core=%d\n", id, attrNameToString(name), getNumNodes( dev ), getCpusPerNode( dev ),id);

	assert( sizeof(double) == len );
	assert( id < getNumNodes( dev ) );


    read_node(&dev->tx2mon.node[id]);
    struct mc_oper_region* pCtrl = &dev->tx2mon.node[id].buf;

	switch( name ) {
	case PWR_ATTR_FREQ:
		*(double*) value = pCtrl->freq_socs; 
		break;
	case PWR_ATTR_TEMP:
		*(double*) value = to_c(pCtrl->tmon_soc_avg);
		break;
	case PWR_ATTR_POWER:
		*(double*) value = to_w(pCtrl->pwr_core);
		*(double*) value += to_w(pCtrl->pwr_sram);
		*(double*) value += to_w(pCtrl->pwr_mem);
		*(double*) value += to_w(pCtrl->pwr_soc);
		*(double*) value += 12;
		break;
	case PWR_ATTR_VOLTAGE:
		*(double*) value = to_v(pCtrl->v_soc);
		break;
	case PWR_ATTR_ENERGY:
		*(double*) value = fd->energy;
		break;
	default:
		assert(0);
	}
    if ( time ) {
		*time = getTimeSec();
    }
    return PWR_RET_SUCCESS;
}

static int readDev( pwr_socmonFdInfo_t* fd, PWR_ObjType objType, int id, PWR_AttrName name, void *value, unsigned int len, PWR_Time *time ) {

	DBGP("obj=%s id=%d attr=%s\n", objTypeToString(objType), id, attrNameToString(name));

	switch ( objType ) {
	case PWR_OBJ_SOCKET:
		return readSocket( fd, id, name, value, len, time );
	case PWR_OBJ_CORE:
		return readCore( fd, id, name, value, len, time );
	case PWR_OBJ_NODE:
		return readNode( fd, id, name, value, len, time );
	default:
		assert(0);
	}
}

static int setCoreFreq( pwr_socmonDevInfo_t* devInfo, int core, double freq ) {
	DBGP("core=%d value=%f\n", core, freq );
	fprintf( devInfo->scaling_setspeed_fd[core], "%d", (int) freq ); 
}

static int writeCore( pwr_socmonFdInfo_t* fd, int id, PWR_AttrName name, void *value, unsigned int len ) {
	pwr_socmonDevInfo_t* dev = fd->devInfo;
    int soc = getNode( dev, id );
    int core= getCore( dev, id );
	DBGP("coreId=%d attr=%s numSoc=%d numCores=%d soc=%d core=%d\n", id, attrNameToString(name),
						getNumNodes( dev ), getCpusPerNode( dev ),soc,core);

	assert( sizeof(double) == len );
	assert( soc < getNumNodes( dev ) );
	assert( core < getCpusPerNode( dev ) );

	read_node(&dev->tx2mon.node[soc]);
	struct mc_oper_region* pCtrl = &dev->tx2mon.node[soc].buf;	

	switch( name ) {
	case PWR_ATTR_FREQ:
		return setCoreFreq( dev, id, *(double*) value );
		break;
	default:
		assert(0);
	}

	return PWR_RET_FAILURE;
}



static int writeSocket( pwr_socmonFdInfo_t* fd, int id, PWR_AttrName name, void *value, unsigned int len ) {
	pwr_socmonDevInfo_t* dev = fd->devInfo;

	DBGP("socId=%d attr=%s numSoc=%d soc=%d core=%d\n", id, attrNameToString(name), getNumNodes( dev ), getCpusPerNode( dev ) ,id);

	assert( sizeof(double) == len );
	assert( id < getNumNodes( dev ) );

	read_node(&dev->tx2mon.node[id]);
	struct mc_oper_region* pCtrl = &dev->tx2mon.node[id].buf;	

	switch( name ) {
	case PWR_ATTR_FREQ:
		break;
	default:
		assert(0);
	}
	return PWR_RET_SUCCESS;
}

static int writeDev( pwr_socmonFdInfo_t* fd, PWR_ObjType objType, int id, PWR_AttrName name, void *value, unsigned int len ){
	DBGP("obj=%s id=%d attr=%s\n", objTypeToString(objType), id, attrNameToString(name));

	switch ( objType ) {
	case PWR_OBJ_SOCKET:
		return writeSocket( fd, id, name, value, len );
	case PWR_OBJ_CORE:
		return writeCore( fd, id, name, value, len );
	default:
		assert(0);
	}
}
