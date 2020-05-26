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

#include <linux/limits.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <mc_oper_region.h>

#include "pwr_dev.h"
#include "util.h"

#define PATH_T99MON_DEV     "/sys/bus/platform/devices/tx2mon"
#define PATH_T99MON_NODE0   PATH_T99MON_DEV "/node0_raw"
#define PATH_T99MON_NODE1   PATH_T99MON_DEV "/node1_raw"
#define PATH_T99MON_SOCINFO PATH_T99MON_DEV "/socinfo"

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


struct pwr_tx2monFdInfo;

typedef struct {
	struct tx2mon tx2mon;
	pthread_t thread;
	useconds_t sleep_us;
	int done;
	struct pwr_tx2monFdInfo** energyObjs;
	double lastSample;
} pwr_tx2monDevInfo_t;

typedef struct pwr_tx2monFdInfo {
	PWR_ObjType objType;
	int index;
	int socket;
	pwr_tx2monDevInfo_t* devInfo;
	double energy;
} pwr_tx2monFdInfo_t;

static double getTime() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	double value; 
	value = tv.tv_sec * 1000000000;
	value += tv.tv_usec * 1000;
	return value;
}

static int readDev( pwr_tx2monFdInfo_t*, PWR_ObjType, int socket, int index, PWR_AttrName name, void *value, unsigned int len, PWR_Time *time );
static int writeDev( pwr_tx2monFdInfo_t*, PWR_ObjType, int socket, int index, PWR_AttrName name, void *value, unsigned int len );

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


static double getTimeSec()
{
	return getTime() / 1000000000.0;
}
  

static char* splitString( char* str, char c ) {
	int index = strlen(str) - 1;
	char* str2 = NULL;
	while ( index > 0  && str[index] != c ) {
		--index;
	}
	if ( index > -1 ) {
		str[index] = 0;
		str2 = str + 1 + index;
	}
	return str2;
}

PWR_ObjType getObjType( char * name ) {
	if ( 0 == strcmp(name,"core") ) {
		return PWR_OBJ_CORE;
	} else if ( 0 == strcmp(name,"tx2_core") ) {
		return PWR_OBJ_TX2_CORE;
	} else if ( 0 == strcmp(name,"tx2_sram") ) {
		return PWR_OBJ_TX2_SRAM;
	} else if ( 0 == strcmp(name,"tx2_mem") ) {
		return PWR_OBJ_TX2_MEM;
	} else if ( 0 == strcmp(name,"tx2_soc") ) {
		return PWR_OBJ_TX2_SOC;
	} else {
		fprintf(stderr, "%s() unknown type %s\n",__func__,name);
		assert(0);
	}
}

static void findNameIndex( const char* str, char* name, int* index ) {
	int pos = strlen(str) -1;
	while ( pos > -1 && isdigit( str[pos] ) ) { --pos; } 
	assert( pos > -1 );
	++pos;
	*index = atoi(str+pos);
	memcpy(name,str,pos);
	name[pos] = 0;
}

static pwr_fd_t pwr_tx2mon_dev_open( plugin_devops_t* ops, const char *openstr )
{
	DBGP("openstr=`%s`\n",openstr);
	char devName[100];
	assert( strlen(openstr) + 1 < 100 );
	strcpy( devName, openstr);
	pwr_tx2monFdInfo_t *fdInfo = malloc( sizeof( pwr_tx2monFdInfo_t ) );
	fdInfo->devInfo = ops->private_data;
	fdInfo->energy = 0;

	char* obj = splitString( devName, '.' );
	char objName[100]; 
	int index;
	findNameIndex( obj, objName, &index );

	if ( 0 == strcmp( objName, "socket" ) ) {
		fdInfo->socket=index; 
		fdInfo->index=-1;
		fdInfo->objType = PWR_OBJ_SOCKET;
	} else {
		fdInfo->index=index;
		fdInfo->objType = getObjType(objName);

		char* obj2 = splitString( devName, '.' );
		findNameIndex( obj2, objName, &fdInfo->socket );
		assert( 0 == strcmp( objName, "socket" ) );
	}

	DBGP("socket=%d index=%d type=%s\n", fdInfo->socket, fdInfo->index, objTypeToString(fdInfo->objType) );	

	if ( fdInfo->objType == PWR_OBJ_SOCKET ) {
		DBGP("calculate energy\n");
		fdInfo->devInfo->energyObjs[fdInfo->socket] = fdInfo;
	}

	return fdInfo;
}

static int pwr_tx2mon_dev_close( pwr_fd_t fd )
{
	pwr_tx2monFdInfo_t* info = fd; 
	if ( info->objType == PWR_OBJ_SOCKET ) {
		DBGP("calculate energy\n");
		info->devInfo->energyObjs[info->socket] = NULL;
	}
    free( fd );
    return 0;
}

static int pwr_tx2mon_dev_read( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len, PWR_Time* ts )
{
	pwr_tx2monFdInfo_t* info = fd;
	return readDev( fd, info->objType, info->socket, info->index, type, ptr, len, ts );
}

static int pwr_tx2mon_dev_write( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len )
{
	pwr_tx2monFdInfo_t* info = fd;
	return writeDev( fd, info->objType, info->socket, info->index, type, ptr, len );
}

static plugin_devops_t devOps = {
	.open   = pwr_tx2mon_dev_open, 
	.close  = pwr_tx2mon_dev_close,
	.read   = pwr_tx2mon_dev_read,
	.write  = pwr_tx2mon_dev_write,
	.readv  = NULL,
	.writev = NULL,
	.time   = NULL,
	.clear  = NULL,
	.log_start = NULL,
	.log_stop = NULL,
	.get_samples = NULL,
};

static pwr_tx2monDevInfo_t* tx2monInit( const char* devName);
static void tx2monFini( pwr_tx2monDevInfo_t* devInfo );

static int initSampleThread( pwr_tx2monDevInfo_t* devInfo );
static int finiSampleThread( pwr_tx2monDevInfo_t* devInfo );

static plugin_devops_t* pwr_tx2mon_dev_init( const char *initstr )
{
	pwr_tx2monDevInfo_t* devInfo = tx2monInit( initstr );
	DBGP("\n");

	if( initSampleThread( devInfo ) ) {
		free( devInfo );
		return NULL;
	}

	if ( NULL == devInfo ) {
		return NULL;
	}
	
	plugin_devops_t* ops = malloc(sizeof(*ops));
	*ops = devOps;

	ops->private_data = devInfo;

	return ops;
}

static int pwr_tx2mon_dev_final( plugin_devops_t *ops )
{
	pwr_tx2monDevInfo_t* devInfo = ops->private_data;
	DBGP("\n");
	finiSampleThread( devInfo );
	tx2monFini( devInfo ); 
	free( ops );
	return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_tx2mon_dev_init, 
    .final  = pwr_tx2mon_dev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

//
// Start of plugin meta data
//

// Any thing can co here as long as it does not clash with other plugins
static int pwr_tx2mon_getPluginName( size_t len, char* buf )
{
    strncpy(buf,"tx2mon",len);
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

static int getNodePower( double* value );

static void initMeta()
{
	int i;
	for ( i=0; i < PWR_NUM_OBJ_TYPES; i++ ) {
		_metaInfo.metaObj[i].numAttrs = 0; 
	}
	_metaInfo.num_objects = 0;

	addAttr( &_metaInfo.metaObj[PWR_OBJ_CORE], PWR_ATTR_FREQ );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_CORE], PWR_ATTR_TEMP );
	++_metaInfo.num_objects;

	addAttr( &_metaInfo.metaObj[PWR_OBJ_TX2_CORE], PWR_ATTR_POWER );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_TX2_CORE], PWR_ATTR_VOLTAGE );
	++_metaInfo.num_objects;
	addAttr( &_metaInfo.metaObj[PWR_OBJ_TX2_SRAM], PWR_ATTR_POWER );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_TX2_SRAM], PWR_ATTR_VOLTAGE );
	++_metaInfo.num_objects;
	addAttr( &_metaInfo.metaObj[PWR_OBJ_TX2_MEM], PWR_ATTR_POWER );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_TX2_MEM], PWR_ATTR_VOLTAGE );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_TX2_MEM], PWR_ATTR_FREQ );
	++_metaInfo.num_objects;
	addAttr( &_metaInfo.metaObj[PWR_OBJ_TX2_SOC], PWR_ATTR_POWER );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_TX2_SOC], PWR_ATTR_VOLTAGE );
	++_metaInfo.num_objects;

	addAttr( &_metaInfo.metaObj[PWR_OBJ_SOCKET], PWR_ATTR_TEMP );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_SOCKET], PWR_ATTR_FREQ );

	char* envPtr = getenv( "PWR_TX2MON_POLL_US" );
	if ( ! ( envPtr &&  0 == atoi( envPtr ) ) ) {
		addAttr( &_metaInfo.metaObj[PWR_OBJ_SOCKET], PWR_ATTR_ENERGY );
	}
	++_metaInfo.num_objects;
}

// What objects does this plugin support?
static int pwr_tx2mon_readObjs(  int numReq, PWR_ObjType* ptr )
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
// this is the size of the array referenced by pwr_tx2mon_readObjs() 
static int pwr_tx2mon_numObjs( )
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
static int pwr_tx2mon_readAttrs( PWR_ObjType type, int numReq, PWR_AttrName* ptr )
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
// this is the size of the array referenced by pwr_tx2mon_readAttr()
static int pwr_tx2mon_numAttrs( PWR_ObjType type )
{
	DBGP("%s has %d attributes\n", objTypeToString(type), _metaInfo.metaObj[type].numAttrs);
	return _metaInfo.metaObj[type].numAttrs;
}

// a plugin device can be initialized multiple times, once for each
// object type, however this is not necessry, you can have one device
// to handle all object types
static int pwr_tx2mon_getDevName(PWR_ObjType type, size_t len, char* buf )
{
	strncpy(buf,"pwr_tx2mon_dev0", len );
	DBGP("type=%s name=`%s`\n",objTypeToString(type),buf);
	return 0;
}

// Create the device initialized string for the specified dev. The name
// was returned the the framework by pwr_tx2mon_getDevName() 
static int pwr_tx2mon_getDevInitStr( const char* devName,
                        size_t len, char* buf )
{
	//strncpy(buf,"/dev/tx2mon",len);
	strncpy(buf,"",len);
	DBGP("dev=`%s` str=`%s`\n",devName, buf );
	return 0;
}

// a device can be opened multiple times, get the info to pass to the 
// open call for this object type 
static int pwr_tx2mon_getDevOpenStr(PWR_ObjType type,
                        int global_index, size_t len, char* buf )
{
 	snprintf( buf, len, "%d %d", type, global_index);
	DBGP("type=%s index=%d str=`%s`\n",objTypeToString(type),global_index,buf);
	return 0;
}

static plugin_meta_t meta = {
	.numObjs = pwr_tx2mon_numObjs,
	.numAttrs = pwr_tx2mon_numAttrs,
	.readObjs = pwr_tx2mon_readObjs,
	.readAttrs = pwr_tx2mon_readAttrs,
	.getDevName = pwr_tx2mon_getDevName,
	.getDevOpenStr = pwr_tx2mon_getDevOpenStr,
	.getDevInitStr = pwr_tx2mon_getDevInitStr,
	.getPluginName = pwr_tx2mon_getPluginName,
};

plugin_meta_t* getMeta() {
	initMeta();
	return &meta;
}

static int inline getNode( pwr_tx2monDevInfo_t* dev, int id ) {
	return id / dev->tx2mon.node[0].cores;
}
static int inline getCore( pwr_tx2monDevInfo_t* dev, int id ) {
	return id % dev->tx2mon.node[0].cores;
}
static int inline getNumNodes( pwr_tx2monDevInfo_t* dev ) {
	return dev->tx2mon.nodes;
}
static int inline getCpusPerNode( pwr_tx2monDevInfo_t* dev ) {
	return dev->tx2mon.node[0].cores;
}

static void energy( pwr_tx2monDevInfo_t* devInfo ) {
	struct timeval ts;
	gettimeofday( &ts, NULL );
	double now = getTimeSec();
	DBGP("time %f\n",now);	
	int i;
	for ( i = 0; i < getNumNodes( devInfo ); i++ ) {
		
		pwr_tx2monFdInfo_t* fd = devInfo->energyObjs[i];
		if ( fd && devInfo->lastSample ) {
			double value;
			pwr_tx2mon_dev_read( fd, PWR_ATTR_POWER, &value, sizeof(value), NULL );
			fd->energy += (now - devInfo->lastSample ) * value;
			DBGP("POWER %f, ENERGY %f\n",value, fd->energy);	
		}
	} 
	devInfo->lastSample = now;
}

static void* thread( void* info ) {
	DBGP("\n");	
	pwr_tx2monDevInfo_t* devInfo = info;
	while( ! devInfo->done ) {
		usleep( devInfo->sleep_us );
		energy( devInfo );
	}
}

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

static pwr_tx2monDevInfo_t* tx2monInit( const char* devName)
{
	int fd;
	int node_id;
	pwr_tx2monDevInfo_t* devInfo = malloc( sizeof( pwr_tx2monDevInfo_t ) );

	init_socinfo( &devInfo->tx2mon );

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

	return devInfo; 
}

static int initSampleThread( pwr_tx2monDevInfo_t* devInfo ) 
{
	devInfo->done = 0;
	devInfo->lastSample = 0;
	devInfo->energyObjs = malloc( sizeof( pwr_tx2monFdInfo_t* ) * devInfo->tx2mon.nodes );
	bzero( devInfo->energyObjs, sizeof( pwr_tx2monFdInfo_t* ) * devInfo->tx2mon.nodes );
	
	devInfo->sleep_us  = 10000;

	char *envPtr;
	if ( envPtr = getenv( "PWR_TX2MON_POLL_US" ) ) {
		devInfo->sleep_us = atoi( envPtr );
	}

	if ( devInfo->sleep_us == 0 ) {
		return 0;
	}
	envPtr = getenv( "PWR_TX2MON_THREAD_AFFINITY");
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

static int finiSampleThread( pwr_tx2monDevInfo_t* devInfo ) 
{
	if ( devInfo->sleep_us ) {
		devInfo->done = 1;
		pthread_join( devInfo->thread, NULL ); 
	}
	free( devInfo->energyObjs );
}

static void tx2monFini( pwr_tx2monDevInfo_t* devInfo ) {
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
    if ( CMD_STATUS_READY(op->cmd_status) == 0)
        return 0;
    return 1;
}

static int readCore( pwr_tx2monFdInfo_t* fd, int soc, int core, PWR_AttrName name, void *value, unsigned int len, PWR_Time *time ) {

	pwr_tx2monDevInfo_t* dev = fd->devInfo;
	DBGP("attr=%s numSoc=%d numCores=%d soc=%d core=%d\n", attrNameToString(name), 
										getNumNodes( dev ), getCpusPerNode( dev ),soc,core);

	assert( sizeof(double) == len );
	assert( soc < getNumNodes( dev ) );
	assert( core < getCpusPerNode( dev ) );

	read_node(&dev->tx2mon.node[soc]);
	struct mc_oper_region* pCtrl = &dev->tx2mon.node[soc].buf;	

	switch( name ) {
	case PWR_ATTR_FREQ:
		*(double*) value = pCtrl->freq_cpu[core] * 1000000;
		break;
	case PWR_ATTR_TEMP:
		*(double*) value = to_c( pCtrl->tmon_cpu[core] );
		break;
	default:
		assert(0);
	}
    if ( time ) {
		*time = getTime();
    }
    return PWR_RET_SUCCESS;
}

static int readTX2( pwr_tx2monFdInfo_t* fd, int socket, PWR_AttrName name, void *value, unsigned int len, PWR_Time *time ) {
	pwr_tx2monDevInfo_t* dev = fd->devInfo;

	DBGP("socket=%d attr=%s numSoc=%d soc=%d\n", socket, attrNameToString(name), getNumNodes( dev ), getCpusPerNode( dev ));

	assert( sizeof(double) == len );
	assert( socket < getNumNodes( dev ) );

	read_node(&dev->tx2mon.node[socket]);
	struct mc_oper_region* pCtrl = &dev->tx2mon.node[socket].buf;

	switch( name ) {
	case PWR_ATTR_FREQ:
		switch ( fd->objType ) {
		case PWR_OBJ_TX2_MEM:
			*(double*) value += pCtrl->freq_mem_net * 1000000;
			break;
		default:
			assert(0);
		}
		break;
	case PWR_ATTR_VOLTAGE:
		switch ( fd->objType ) {
		case PWR_OBJ_TX2_CORE:
			*(double*) value = to_w(pCtrl->v_core);
			break;
		case PWR_OBJ_TX2_SRAM:
			*(double*) value += to_w(pCtrl->v_sram);
			break;
		case PWR_OBJ_TX2_MEM:
			*(double*) value += to_w(pCtrl->v_mem);
			break;
		case PWR_OBJ_TX2_SOC:
			*(double*) value += to_w(pCtrl->v_soc);
			break;
		}
		break;

	case PWR_ATTR_POWER:
		switch ( fd->objType ) {
		case PWR_OBJ_TX2_CORE:
			*(double*) value = to_w(pCtrl->pwr_core);
			break;
		case PWR_OBJ_TX2_SRAM:
			*(double*) value += to_w(pCtrl->pwr_sram);
			break;
		case PWR_OBJ_TX2_MEM:
			*(double*) value += to_w(pCtrl->pwr_mem);
			break;
		case PWR_OBJ_TX2_SOC:
			*(double*) value += to_w(pCtrl->pwr_soc);
			break;
		}
		break;
	default:
		assert(0);
	}
    if ( time ) {
		*time = getTime();
    }
    return PWR_RET_SUCCESS;
}

static int readSocket( pwr_tx2monFdInfo_t* fd, int socket, PWR_AttrName name, void *value, unsigned int len, PWR_Time *time ) {

	pwr_tx2monDevInfo_t* dev = fd->devInfo;

	DBGP("socId=%d attr=%s numSoc=%d soc=%d\n", socket, attrNameToString(name), getNumNodes( dev ), getCpusPerNode( dev ));

	assert( sizeof(double) == len );
	assert( socket < getNumNodes( dev ) );

	read_node(&dev->tx2mon.node[socket]);
	struct mc_oper_region* pCtrl = &dev->tx2mon.node[socket].buf;

	switch( name ) {
	case PWR_ATTR_FREQ:
		*(double*) value = pCtrl->freq_socs * 1000000; 
		break;
	case PWR_ATTR_TEMP:
		*(double*) value = to_c(pCtrl->tmon_soc_avg);
		break;
	case PWR_ATTR_ENERGY:
		*(double*) value = fd->energy;
		break;

	// this is for plugin use
	case PWR_ATTR_POWER:
		*(double*) value = to_w(pCtrl->pwr_core);
		*(double*) value += to_w(pCtrl->pwr_sram);
		*(double*) value += to_w(pCtrl->pwr_mem);
		*(double*) value += to_w(pCtrl->pwr_soc);
		*(double*) value += 12;
        break;
	default:
		assert(0);
	}
    if ( time ) {
		*time = getTime();
    }
    return PWR_RET_SUCCESS;
}

static int readDev( pwr_tx2monFdInfo_t* fd, PWR_ObjType objType, int socket, int index, PWR_AttrName name, void *value, unsigned int len, PWR_Time *time ) {

	DBGP("obj=%s socket=%d index=%d attr=%s\n", objTypeToString(objType), socket, index, attrNameToString(name));

	switch ( objType ) {
	case PWR_OBJ_SOCKET:
		return readSocket( fd, socket, name, value, len, time );
	case PWR_OBJ_TX2_CORE:
	case PWR_OBJ_TX2_SRAM:
	case PWR_OBJ_TX2_MEM:
	case PWR_OBJ_TX2_SOC:
		return readTX2( fd, socket, name, value, len, time );
	case PWR_OBJ_CORE:
		return readCore( fd, socket, index, name, value, len, time );
	default:
		assert(0);
	}
}

static int setCoreFreq( pwr_tx2monDevInfo_t* devInfo, int socket, int core, double freq ) {
	DBGP("socket=%d core=%d value=%f\n", socket, core, freq );
	
#define SYSFS_CPU "/sys/devices/system/cpu"

	char filename[PATH_MAX];
	char cmd[100];
	int cpu = core; 

	if ( socket == 1 ) {
		cpu += 56;
	}
	
	sprintf( filename, "%s/cpu%d/cpufreq/scaling_setspeed",SYSFS_CPU,cpu);

	sprintf( cmd, "echo %d > %s", (int) freq * 1000, filename );
	DBGP("%s\n",cmd);

	int ret = system( cmd );

	if ( ret != 0 ) {
		fprintf(stderr,"ERROR: could not write %f to %s\n",freq,filename);
		exit(-1);
	}
	assert(ret == 0);
}

static int writeCore( pwr_tx2monFdInfo_t* fd, int soc, int core, PWR_AttrName name, void *value, unsigned int len ) {
	pwr_tx2monDevInfo_t* dev = fd->devInfo;
	DBGP("attr=%s numSoc=%d numCores=%d soc=%d core=%d\n", attrNameToString(name),
						getNumNodes( dev ), getCpusPerNode( dev ),soc,core);

	assert( sizeof(double) == len );
	assert( soc < getNumNodes( dev ) );
	assert( core < getCpusPerNode( dev ) );

	read_node(&dev->tx2mon.node[soc]);
	struct mc_oper_region* pCtrl = &dev->tx2mon.node[soc].buf;	

	switch( name ) {
	case PWR_ATTR_FREQ:
		return setCoreFreq( dev, soc, core, *(double*) value );
		break;
	default:
		assert(0);
	}

	return PWR_RET_FAILURE;
}



static int writeSocket( pwr_tx2monFdInfo_t* fd, int id, PWR_AttrName name, void *value, unsigned int len ) {
	pwr_tx2monDevInfo_t* dev = fd->devInfo;

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

static int writeDev( pwr_tx2monFdInfo_t* fd, PWR_ObjType objType, int socket, int index, PWR_AttrName name, void *value, unsigned int len ){
	DBGP("obj=%s socket=%d index=%d attr=%s\n", objTypeToString(objType), socket, index, attrNameToString(name));

	switch ( objType ) {
	case PWR_OBJ_SOCKET:
		return writeSocket( fd, socket, name, value, len );
	case PWR_OBJ_CORE:
		return writeCore( fd, socket, index, name, value, len );
	default:
		assert(0);
	}
}
