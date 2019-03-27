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
#include <sys/time.h>
#include <assert.h>
#include <stdio.h>

#include "pwr_dev.h"
#include "util.h"
#include "occ_sensor.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static void fatal( const char* msg ) {
	fprintf(stderr, "Fatal: %s\n",msg );
	exit(1);
}

typedef struct pwr_occSensorPrivateInfo {
	int fd;
	char* buf;
} pwr_occSensorPrivateInfo_t;

typedef struct pwr_occSensorFdInfo {
	OCC_Sensors sensor;
	pwr_occSensorPrivateInfo_t* pd;
} pwr_occSensorFdInfo_t;


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
  
static pwr_fd_t pwr_occSensor_dev_open( plugin_devops_t* ops, const char *openstr )
{
	DBGP("openstr=`%s`\n",openstr);
	PWR_ObjType objType;
	int id;
	pwr_occSensorFdInfo_t *tmp = malloc( sizeof( pwr_occSensorFdInfo_t ) );
	tmp->pd = ops->private_data;
        sscanf(openstr, "%d %d", &objType, &id );
        DBGP("open obj=%s id=%d\n", objTypeToString(objType), id);
	switch( objType ) {
	case PWR_OBJ_NODE:
		tmp->sensor = System;
		break;
	case PWR_OBJ_SOCKET:
		if ( 0 == id ) {
			tmp->sensor = Socket_0;
		} else if ( 1 == id ) {
			tmp->sensor = Socket_1;
		} else {
			assert(0);
		}
		break;
	case PWR_OBJ_GPU:
		if ( 0 == id ) {
			tmp->sensor = GPU_0;
		} else if ( 1 == id ) {
			tmp->sensor = GPU_1;
		} else {
			assert(0);
		}
		break;
	case PWR_OBJ_MEM:
		if ( 0 == id ) {
			tmp->sensor = Memory_0;
		} else if ( 1 == id ) {
			tmp->sensor = Memory_1;
		} else {
			assert(0);
		}
		break;
	} 
	return tmp;
}

static int pwr_occSensor_dev_close( pwr_fd_t fd )
{
	pwr_occSensorFdInfo_t* info = fd; 
	free( fd );
	return 0;
}

static int pwr_occSensor_dev_read( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len, PWR_Time* ts )
{
	int ret = PWR_RET_FAILURE; 
	pwr_occSensorFdInfo_t* info = fd;
	

        OCC_readData( info->pd->fd, info->sensor, info->pd->buf, OCC_dataSize() );
	switch( type ) {
	  case PWR_ATTR_POWER:
		*(double*) ptr = (double) OCC_readPower( info->pd->buf, info->sensor ); 
		ret = PWR_RET_SUCCESS;
		break;
	  case PWR_ATTR_ENERGY:
		*(double*) ptr = (double) OCC_readEnergy( info->pd->buf, info->sensor ); 
		ret = PWR_RET_SUCCESS;
		break;
	  default:
		break;
	}

	if ( ts ) {
		*ts = getTime();
	}
	return ret;
}

static int pwr_occSensor_dev_write( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len )
{
	return PWR_RET_FAILURE;
}

static plugin_devops_t devOps = {
	.open   = pwr_occSensor_dev_open, 
	.close  = pwr_occSensor_dev_close,
	.read   = pwr_occSensor_dev_read,
	.write  = pwr_occSensor_dev_write,
	.readv  = NULL,
	.writev = NULL,
	.time   = NULL,
	.clear  = NULL,
	.log_start = NULL,
	.log_stop = NULL,
	.get_samples = NULL,
};

static plugin_devops_t* pwr_occSensor_dev_init( const char *initstr )
{
	DBGP("\n");

	pwr_occSensorPrivateInfo_t* pd = malloc( sizeof( pwr_occSensorPrivateInfo_t ) );
	int i;
        pd->fd = open("/sys/firmware/opal/exports/occ_inband_sensors", O_RDONLY);
        if (pd->fd < 0) {
                printf("Failed to open occ_inband_sensors file\n");
		free(pd);
                return NULL;
        }

        pd->buf = (char*) malloc( OCC_dataSize() );

	plugin_devops_t* ops = malloc(sizeof(*ops));
	*ops = devOps;
	ops->private_data = pd; 
	return ops;
}

static int pwr_occSensor_dev_final( plugin_devops_t *ops )
{
	DBGP("\n");
	free( ops );
	return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_occSensor_dev_init, 
    .final  = pwr_occSensor_dev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

//
// Start of plugin meta data
//

// Any thing can co here as long as it does not clash with other plugins
static int pwr_occSensor_getPluginName( size_t len, char* buf )
{
    strncpy(buf,"occ_sensor",len);
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
	_metaInfo.num_objects = 0;

	int cnt = 0;

        int fd = open("/sys/firmware/opal/exports/occ_inband_sensors", O_RDONLY);
        if (fd < 0) {
                printf("Failed to open occ_inband_sensors file\n");
                return;
        }

        char*  buf = (char*) malloc( OCC_dataSize() );

	int ret = OCC_readData( fd, System, buf, OCC_dataSize() );
	free( buf );
	if ( ret == -1 ) {
		return;
	}

	addAttr( &_metaInfo.metaObj[PWR_OBJ_NODE], PWR_ATTR_POWER );
	addAttr( &_metaInfo.metaObj[PWR_OBJ_NODE], PWR_ATTR_ENERGY );
	++_metaInfo.num_objects;

	addAttr( &_metaInfo.metaObj[PWR_OBJ_SOCKET], PWR_ATTR_POWER );
	++_metaInfo.num_objects;

	addAttr( &_metaInfo.metaObj[PWR_OBJ_GPU], PWR_ATTR_POWER );
	++_metaInfo.num_objects;

	addAttr( &_metaInfo.metaObj[PWR_OBJ_MEM], PWR_ATTR_POWER );
	++_metaInfo.num_objects;
}

// What objects does this plugin support?
static int pwr_occSensor_readObjs(  int numReq, PWR_ObjType* ptr )
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
// this is the size of the array referenced by pwr_occSensor_readObjs() 
static int pwr_occSensor_numObjs( )
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
static int pwr_occSensor_readAttrs( PWR_ObjType type, int numReq, PWR_AttrName* ptr )
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
// this is the size of the array referenced by pwr_occSensor_readAttr()
static int pwr_occSensor_numAttrs( PWR_ObjType type )
{
	DBGP("%s has %d attributes\n", objTypeToString(type), _metaInfo.metaObj[type].numAttrs);
	return _metaInfo.metaObj[type].numAttrs;
}

// a plugin device can be initialized multiple times, once for each
// object type, however this is not necessry, you can have one device
// to handle all object types
static int pwr_occSensor_getDevName(PWR_ObjType type, size_t len, char* buf )
{
	strncpy(buf,"pwr_occSensor_dev0", len );
	DBGP("type=%d name=`%s`\n",type,buf);
	return 0;
}

// Create the device initialized string for the specified dev. The name
// was returned the the framework by pwr_occSensor_getDevName() 
static int pwr_occSensor_getDevInitStr( const char* devName,
                        size_t len, char* buf )
{
	//strncpy(buf,"/dev/occSensor",len);
	strncpy(buf,"",len);
	DBGP("dev=`%s` str=`%s`\n",devName, buf );
	return 0;
}

// a device can be opened multiple times, get the info to pass to the 
// open call for this object type 
static int pwr_occSensor_getDevOpenStr(PWR_ObjType type,
                        int global_index, size_t len, char* buf )
{
 	snprintf( buf, len, "%d %d", type, global_index);
	DBGP("type=%d index=%d str=`%s`\n",type,global_index,buf);
	return 0;
}

static plugin_meta_t meta = {
	.numObjs = pwr_occSensor_numObjs,
	.numAttrs = pwr_occSensor_numAttrs,
	.readObjs = pwr_occSensor_readObjs,
	.readAttrs = pwr_occSensor_readAttrs,
	.getDevName = pwr_occSensor_getDevName,
	.getDevOpenStr = pwr_occSensor_getDevOpenStr,
	.getDevInitStr = pwr_occSensor_getDevInitStr,
	.getPluginName = pwr_occSensor_getPluginName,
};

plugin_meta_t* getMeta() {
	initMeta();
	return &meta;
}
