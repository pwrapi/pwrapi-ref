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

#define PATH_MAX 4096
#include <sys/timeb.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#define _GNU_SOURCE
#include <string.h>
#include <sys/time.h>

#include "pwr_dev.h"
#include "util.h"

typedef struct {
    char* name;
    char  energy[PATH_MAX];
    long long startValue;
} zone_t; 

typedef struct {
    zone_t zone;

    int numZones;
    zone_t* zones;

} pkg_t;

typedef struct {
    int numPkgs;
    pkg_t* pkgs;
} powercapDevInfo_t;

typedef struct {
    PWR_ObjType type;
    pkg_t*      pkg;
} open_t;

static double getTime() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
	double value; 
    value = tv.tv_sec * 1000000000;
    value += tv.tv_usec * 1000;
	return value;
}
  
static pwr_fd_t powercap_dev_open( plugin_devops_t* ops, const char *openstr )
{
    DBGP("openstr=`%s`\n",openstr);
    open_t* fd = malloc( sizeof( open_t ) );

    powercapDevInfo_t *info = (powercapDevInfo_t*) ops->private_data;

    int   global_index;
    sscanf( openstr, "%d %d", &fd->type, &global_index ); 
    DBGP("type=%d global_index=%d\n",fd->type,global_index);
    
    fd->pkg = info->pkgs + global_index;

    return fd; 
}

static int powercap_dev_close( pwr_fd_t fd )
{
    DBGP("\n");
    free( fd );

    return 0;
}

static long long readValue( const char* path )
{
    long long value;
    FILE* fp = fopen( path, "r" );
    assert(fp);
    DBGP("%s\n",path);
    int rc = fscanf( fp, "%lld", &value );
    assert( EOF != rc );
    fclose(fp);
    return value;
}
static long long readValue2( zone_t* zone )
{
    long long value = readValue( zone->energy );

    return value - zone->startValue;
}

static int powercap_dev_read( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len, PWR_Time* ts )
{
    int rc;
    long long value;
    open_t* info = (open_t*) fd;

    DBGP("type=%s\n", attrNameToString(type) );
    DBGP("pkg name %s %s\n", info->pkg->zone.name, objTypeToString(info->type));

    switch ( info->type ) {
      case PWR_OBJ_MEM:
        
        value = readValue2( info->pkg->zones + 0 );
        break;

      case PWR_OBJ_SOCKET:
        {
            long long value0 = readValue2( info->pkg->zones + 0 );
            long long value1 = readValue2( &info->pkg->zone );
            value = value1 - value0;
        }
        break;

      default:
        break;
    }
    
    DBGP("value=%lld\n",value);

    if ( ts ) {
		*ts = getTime();
    }
    *(double*) ptr = (double) value / 1000000.0;

    return PWR_RET_SUCCESS;
}

static int powercap_dev_write( pwr_fd_t fd, PWR_AttrName type, void* ptr, unsigned int len )
{
    DBGP("type=%s %f\n",attrNameToString(type), *(double*)ptr);

    return PWR_RET_FAILURE;
}

static int powercap_dev_readv( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], void* buf,
                        PWR_Time ts[], int status[] )
{
    if ( 1 != arraysize ) {
        return PWR_RET_FAILURE;
    } 

    DBGP("type=%s\n",attrNameToString(attrs[0]));
    int rc = powercap_dev_read( fd, attrs[0], buf, 8, &ts[0] );
    status[0] = rc;

    return rc;
}

static int powercap_dev_writev( pwr_fd_t fd, unsigned int arraysize, const PWR_AttrName attrs[], void* buf, int status[] )
{
    DBGP("\n");
    return PWR_RET_FAILURE;
}

static int powercap_dev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    DBGP("\n");
    return PWR_RET_FAILURE;

    return 0;
}

static int powercap_dev_clear( pwr_fd_t fd )
{
    DBGP("\n");
    return PWR_RET_FAILURE;

    return 0;
}

static int powercap_dev_log_start( pwr_fd_t fd, PWR_AttrName name )
{
    DBGP("\n");
	return PWR_RET_FAILURE;
}

static int powercap_dev_log_stop( pwr_fd_t fd, PWR_AttrName name )
{
    DBGP("\n");
	return PWR_RET_FAILURE;
}

static int powercap_dev_get_samples( pwr_fd_t fd, PWR_AttrName name, 
			PWR_Time* ts, double period, unsigned int* nSamples, void* buf )

{
	DBGP("period=%f samples=%d\n",period,*nSamples);
	return PWR_RET_FAILURE;
}

static plugin_devops_t devOps = {
    .open   = powercap_dev_open, 
    .close  = powercap_dev_close,
    .read   = powercap_dev_read,
    .write  = powercap_dev_write,
    .readv  = powercap_dev_readv,
    .writev = powercap_dev_writev,
    .time   = powercap_dev_time,
    .clear  = powercap_dev_clear,
	.log_start = powercap_dev_log_start,
	.log_stop = powercap_dev_log_stop,
	.get_samples = powercap_dev_get_samples,
};

static void initZone( int pkg, int num, const char* prefix, zone_t* zone )
{
    char name[PATH_MAX];
    sprintf(name,"%s/name", prefix );

    FILE* fp = fopen( name, "r" );
    assert(fp);
    fscanf( fp, "%ms", &zone->name );    
    fclose(fp);
    DBGP("zone=`%s` %s\n",prefix,zone->name);

    sprintf(zone->energy,"%s/energy_uj", prefix );
    DBGP("zone=`%s` %s\n",zone->name,name);
    zone->startValue = readValue( zone->energy );
}

static int findNumZones( const char *prefix )
{
    int num = 0;
    while( 1 ) {
        char name[PATH_MAX];
        sprintf(name,"%s%d/name", prefix, num );
        DBGP("check `%s`\n",name)

        // could use fstat here
        FILE* fp = fopen( name, "r" );
        if ( NULL == fp ) {
            break;
        }
        fclose(fp);
        ++num;
    }; 

    return num;
}


static char* findSuffix( const char* path )
{
    char* end = (char*)path + strlen(path)-1;
    while ( end >= path && *(end--) != '/' );
    return end + 1;
}

static void initPkg( int num, const char* prefix, pkg_t* pkg )
{
    char path[PATH_MAX];
    sprintf(path,"%s/name", prefix );

    FILE* fp = fopen( path, "r" );
    assert(fp);
    fscanf( fp, "%ms", &pkg->zone.name );    
    fclose(fp);

    DBGP("pkg=`%s` %s\n",prefix,pkg->zone.name);

    sprintf(pkg->zone.energy,"%s/energy_uj", prefix );
    pkg->zone.startValue = readValue( pkg->zone.energy );
    
    sprintf(path,"%s/%s:",prefix, findSuffix(prefix) );
    pkg->numZones = findNumZones(path);

    DBGP("%s numZones=%d\n", pkg->zone.name, pkg->numZones);
    pkg->zones = malloc( sizeof( zone_t ) * pkg->numZones );
    int i;
    for ( i = 0; i < pkg->numZones; i++ ) {
        char name[PATH_MAX];
        sprintf(name,"%s/intel-rapl:%d:%d", prefix, num, i );

        initZone( num, i, name, &pkg->zones[i] );
    }
}


static plugin_devops_t* powercap_dev_init( const char *initstr )
{
	plugin_devops_t* ops = malloc(sizeof(*ops));
	*ops = devOps;

	DBGP("initstr='%s'\n",initstr);

    char* prefix = "/sys/devices/virtual/powercap/intel-rapl/intel-rapl:";

	powercapDevInfo_t* info = malloc( sizeof( powercapDevInfo_t ) );

	ops->private_data = info;

    info->numPkgs = findNumZones( prefix );

    if ( 0 == info->numPkgs ) {
        printf("%s() didn't find any power zones\n",__func__);
        free( ops->private_data );
        free( ops );
        return NULL;
    }

    DBGP("num packages %d\n",info->numPkgs);

    info->pkgs = malloc( sizeof( pkg_t ) *  info->numPkgs );

    int i;
    for ( i = 0; i < info->numPkgs; i++ ) {
        char name[PATH_MAX];
        sprintf(name,"%s%d", prefix, i );

        initPkg( i, name, info->pkgs + i );
    }

    return ops;
}

static int powercap_dev_final( plugin_devops_t *ops )
{
    DBGP("\n");
	powercapDevInfo_t* info = ops->private_data;
    int i,j;
    for ( i = 0; i < info->numPkgs; i++ ) {
        for ( j = 0; j < info->pkgs[i].numZones; j++ ) {
            free( info->pkgs[i].zones );
        } 
    }
    free( info->pkgs );
	free( ops->private_data );
    free( ops );
    return 0;
}

static plugin_dev_t dev = {
    .init   = powercap_dev_init, 
    .final  = powercap_dev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

static int powercap_numObjs( )
{
	DBGP("\n");
	return 2;
} 
static int powercap_readObjs(  int i, PWR_ObjType* ptr )
{
	DBGP("\n");
	ptr[0] = PWR_OBJ_SOCKET;
	ptr[1] = PWR_OBJ_MEM;
	return 0;
}

static int powercap_numAttrs( )
{
	DBGP("\n");
	return 1;
}

static int powercap_readAttrs( int i, PWR_AttrName* ptr )
{
	DBGP("\n");
	ptr[0] = PWR_ATTR_ENERGY;
	return 0;
}

static int powercap_getDevName(PWR_ObjType type, size_t len, char* buf )
{
    strncpy(buf,"powercap_dev0", len );
	DBGP("type=%d name=`%s`\n",type,buf);
	return 0;
}

static int powercap_getDevOpenStr(PWR_ObjType type, 
                        int global_index, size_t len, char* buf )
{
    snprintf( buf, len, "%d %d", type, global_index);
	DBGP("type=%d global_index=%d str=`%s`\n",type,global_index,buf);
	return 0;
}

static int powercap_getDevInitStr( const char* name, 
                        size_t len, char* buf )
{
    strncpy(buf,"",len);
	DBGP("dev=`%s` str=`%s`\n",name, buf );
	return 0;
}

static int powercap_getPluginName( size_t len, char* buf )
{
    strncpy(buf,"PowerCap",len);
	return 0;
}

static plugin_meta_t meta = {
	.numObjs = powercap_numObjs,
	.numAttrs = powercap_numAttrs,
	.readObjs = powercap_readObjs,
	.readAttrs = powercap_readAttrs,
    .getDevName = powercap_getDevName,
    .getDevOpenStr = powercap_getDevOpenStr,
    .getDevInitStr = powercap_getDevInitStr,
    .getPluginName = powercap_getPluginName,
};

plugin_meta_t* getMeta() {
    return &meta;
}
