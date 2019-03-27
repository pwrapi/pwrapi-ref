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
#include "lm_sensor.h"

static void fatal( const char* msg ) {
	fprintf(stderr, "Fatal: %s\n",msg );
	exit(1);
}

typedef struct pwr_lmSensorFdInfo {
	PWR_ObjType objType;
	int id;
	SensorInfo* 	sensorInfo[PWR_NUM_ATTR_NAMES];
} pwr_lmSensorFdInfo_t;

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
  
static pwr_fd_t pwr_lmSensor_dev_open( plugin_devops_t* ops, const char *openstr )
{
	DBGP("openstr=`%s`\n",openstr);
	pwr_lmSensorFdInfo_t *tmp = malloc( sizeof( pwr_lmSensorFdInfo_t ) );

	int i;
	int lm_id;
	for ( i = 0; i < PWR_NUM_ATTR_NAMES; i++ ) {
		tmp->sensorInfo[i] = 0;
	}
	
	sscanf(openstr, "%d %d", &tmp->objType, &tmp->id );
	DBGP("open obj=%s id=%d\n", objTypeToString(tmp->objType), tmp->id);

	switch( tmp->objType ) {

	case PWR_OBJ_SOCKET:
        	lm_id = sensorFind("/sys/class/hwmon/hwmon0","power",tmp->id == 0 ? "Chip 0":"Chip 8");
        	assert( lm_id > 0 );
		tmp->sensorInfo[PWR_ATTR_POWER] = sensorInit( System, lm_id, "/sys/class/hwmon/hwmon0","power" );
        	assert( tmp->sensorInfo[PWR_ATTR_POWER] );
		break;

	case PWR_OBJ_NODE:
        	lm_id = sensorFind("/sys/class/hwmon/hwmon0","power","System");
        	assert( lm_id > 0 );
		tmp->sensorInfo[PWR_ATTR_POWER] = sensorInit( System, lm_id, "/sys/class/hwmon/hwmon0","power" );
        	assert( tmp->sensorInfo[PWR_ATTR_POWER] );
		break;

	case PWR_OBJ_MEM:
        	lm_id = sensorFind("/sys/class/hwmon/hwmon0","power",tmp->id == 0 ? "Chip 0 Memory":"Chip 8 Memory");
        	assert( lm_id > 0 );
		tmp->sensorInfo[PWR_ATTR_POWER] = sensorInit( System, lm_id, "/sys/class/hwmon/hwmon0","power" );
        	assert( tmp->sensorInfo[PWR_ATTR_POWER] );
		break;

	case PWR_OBJ_GPU:
        	lm_id = sensorFind("/sys/class/hwmon/hwmon0","power",tmp->id == 0 ? "Chip 0 GPU":"Chip 8 GPU");
        	assert( lm_id > 0 );
		tmp->sensorInfo[PWR_ATTR_POWER] = sensorInit( System, lm_id, "/sys/class/hwmon/hwmon0","power" );
        	assert( tmp->sensorInfo[PWR_ATTR_POWER] );
		break;
	} 

	return tmp;
}

static int pwr_lmSensor_dev_close( pwr_fd_t fd )
{
	pwr_lmSensorFdInfo_t* info = fd; 
	free( fd );
	return 0;
}

static int pwr_lmSensor_dev_read( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len, PWR_Time* ts )
{
	int ret = PWR_RET_FAILURE; 
	pwr_lmSensorFdInfo_t* info = fd;
	switch( type ) {
	  case PWR_ATTR_POWER:
		*(double*) ptr = sensorRead( info->sensorInfo[type] );
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

static int pwr_lmSensor_dev_write( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len )
{
	return PWR_RET_FAILURE;
}

static plugin_devops_t devOps = {
	.open   = pwr_lmSensor_dev_open, 
	.close  = pwr_lmSensor_dev_close,
	.read   = pwr_lmSensor_dev_read,
	.write  = pwr_lmSensor_dev_write,
	.readv  = NULL,
	.writev = NULL,
	.time   = NULL,
	.clear  = NULL,
	.log_start = NULL,
	.log_stop = NULL,
	.get_samples = NULL,
};

static plugin_devops_t* pwr_lmSensor_dev_init( const char *initstr )
{
	DBGP("\n");
	plugin_devops_t* ops = malloc(sizeof(*ops));
	*ops = devOps;
	return ops;
}

static int pwr_lmSensor_dev_final( plugin_devops_t *ops )
{
	DBGP("\n");
	free( ops );
	return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_lmSensor_dev_init, 
    .final  = pwr_lmSensor_dev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

//
// Start of plugin meta data
//

// Any thing can co here as long as it does not clash with other plugins
static int pwr_lmSensor_getPluginName( size_t len, char* buf )
{
    strncpy(buf,"lm_sensor",len);
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

        int ret = sensorFind("/sys/class/hwmon/hwmon0","power","System");
	if ( ret > 0 ) {
		++cnt;
		addAttr( &_metaInfo.metaObj[PWR_OBJ_NODE], PWR_ATTR_POWER );
		//addAttr( &_metaInfo.metaObj[PWR_OBJ_NODE], PWR_ATTR_ENERGY );
	}

	if ( cnt ) {
		++_metaInfo.num_objects;
	}

	cnt = 0;
        ret = sensorFind("/sys/class/hwmon/hwmon0","power","Chip 0");
	if ( ret > 0 ) {
		++cnt;
		addAttr( &_metaInfo.metaObj[PWR_OBJ_SOCKET], PWR_ATTR_POWER );
		//addAttr( &_metaInfo.metaObj[PWR_OBJ_NODE], PWR_ATTR_ENERGY );
	}
	if ( cnt ) {
		++_metaInfo.num_objects;
	}

	cnt = 0;
        ret = sensorFind("/sys/class/hwmon/hwmon0","power","Chip 0 GPU");
	if ( ret > 0 ) {
		++cnt;
		addAttr( &_metaInfo.metaObj[PWR_OBJ_GPU], PWR_ATTR_POWER );
		//addAttr( &_metaInfo.metaObj[PWR_OBJ_NODE], PWR_ATTR_ENERGY );
	}
	if ( cnt ) {
		++_metaInfo.num_objects;
	}

	cnt = 0;
        ret = sensorFind("/sys/class/hwmon/hwmon0","power","Chip 0 Memory");
	if ( ret > 0 ) {
		++cnt;
		addAttr( &_metaInfo.metaObj[PWR_OBJ_MEM], PWR_ATTR_POWER );
		//addAttr( &_metaInfo.metaObj[PWR_OBJ_NODE], PWR_ATTR_ENERGY );
	}
	if ( cnt ) {
		++_metaInfo.num_objects;
	}
}

// What objects does this plugin support?
static int pwr_lmSensor_readObjs(  int numReq, PWR_ObjType* ptr )
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
// this is the size of the array referenced by pwr_lmSensor_readObjs() 
static int pwr_lmSensor_numObjs( )
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
static int pwr_lmSensor_readAttrs( PWR_ObjType type, int numReq, PWR_AttrName* ptr )
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
// this is the size of the array referenced by pwr_lmSensor_readAttr()
static int pwr_lmSensor_numAttrs( PWR_ObjType type )
{
	DBGP("%s has %d attributes\n", objTypeToString(type), _metaInfo.metaObj[type].numAttrs);
	return _metaInfo.metaObj[type].numAttrs;
}

// a plugin device can be initialized multiple times, once for each
// object type, however this is not necessry, you can have one device
// to handle all object types
static int pwr_lmSensor_getDevName(PWR_ObjType type, size_t len, char* buf )
{
	strncpy(buf,"pwr_lmSensor_dev0", len );
	DBGP("type=%d name=`%s`\n",type,buf);
	return 0;
}

// Create the device initialized string for the specified dev. The name
// was returned the the framework by pwr_lmSensor_getDevName() 
static int pwr_lmSensor_getDevInitStr( const char* devName,
                        size_t len, char* buf )
{
	//strncpy(buf,"/dev/lmSensor",len);
	strncpy(buf,"",len);
	DBGP("dev=`%s` str=`%s`\n",devName, buf );
	return 0;
}

// a device can be opened multiple times, get the info to pass to the 
// open call for this object type 
static int pwr_lmSensor_getDevOpenStr(PWR_ObjType type,
                        int global_index, size_t len, char* buf )
{
 	snprintf( buf, len, "%d %d", type, global_index);
	DBGP("type=%d index=%d str=`%s`\n",type,global_index,buf);
	return 0;
}

static plugin_meta_t meta = {
	.numObjs = pwr_lmSensor_numObjs,
	.numAttrs = pwr_lmSensor_numAttrs,
	.readObjs = pwr_lmSensor_readObjs,
	.readAttrs = pwr_lmSensor_readAttrs,
	.getDevName = pwr_lmSensor_getDevName,
	.getDevOpenStr = pwr_lmSensor_getDevOpenStr,
	.getDevInitStr = pwr_lmSensor_getDevInitStr,
	.getPluginName = pwr_lmSensor_getPluginName,
};

plugin_meta_t* getMeta() {
	initMeta();
	return &meta;
}
