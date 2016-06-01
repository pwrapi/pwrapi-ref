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

#define USE_SYSTIME

#include <assert.h>
#include "pwr_micdev.h"
#include "pwr_dev.h"
#include <miclib.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#ifndef USE_SYSTIME
#include <sys/time.h>
#endif

#define KNCID 0x2250
#define KNLID 0x225c

typedef enum {
    INTEL_LAYER_INVALID = -1,
    INTEL_LAYER_WIN0 = 0,
    INTEL_LAYER_WIN1,
    INTEL_LAYER_PCIE,
    INTEL_LAYER_INST,
	INTEL_LAYER_MXIN,
	INTEL_LAYER_2X3,
	INTEL_LAYER_2X4,
	INTEL_LAYER_CORP,
	INTEL_LAYER_CORA,
	INTEL_LAYER_CORV,
	INTEL_LAYER_UNCP,
	INTEL_LAYER_UNCA,
	INTEL_LAYER_UNCV,
	INTEL_LAYER_MEMP,
	INTEL_LAYER_MEMA,
	INTEL_LAYER_MEMV
} layer_t;

typedef struct {
	uint32_t limitphysical; /* physical power limit */
    uint32_t limitlow; /* low mark power limit */
    uint32_t limithigh; /* high mark power limit */
	uint32_t window0; /* time of window0 */
	uint32_t window1; /* time of window1 */
} power_t;

typedef struct {
	struct mic_device *mdh;
	int mic, cores;
	uint32_t freq;
	char *device_id;
	power_t power_specs;
} pwr_micdev_t;

typedef struct {
    int numMics;
    pwr_micdev_t **mics;    
} private_data_t;

typedef struct {
    pwr_micdev_t *dev;
    int numLayers;
    layer_t layers[16];
} pwr_micfd_t;

#define PWR_MICFD(X) ((pwr_micfd_t *)(X))

static plugin_devops_t devops = {
    .open         = pwr_micdev_open,
    .close        = pwr_micdev_close,
    .read         = pwr_micdev_read,
    .write        = pwr_micdev_write,
    .readv        = pwr_micdev_readv,
    .writev       = pwr_micdev_writev,
    .time         = pwr_micdev_time,
    .clear        = pwr_micdev_clear,
#if 0
    .stat_get     = pwr_dev_stat_get,
    .stat_start   = pwr_dev_stat_start,
    .stat_stop    = pwr_dev_stat_stop,
    .stat_clear   = pwr_dev_stat_clear,
#endif
    .private_data = 0x0
};

static layer_t micdev_parse_open( const char *name )
{
    layer_t layer = INTEL_LAYER_INVALID;
    if( !strcmp( name, "win0" ) ) layer = INTEL_LAYER_WIN0;
    else if( !strcmp( name, "win1" ) ) layer = INTEL_LAYER_WIN1;
    else if( !strcmp( name, "pci" ) || !strcmp( name, "pcie" ) ) layer = INTEL_LAYER_PCIE;
    else if( !strcmp( name, "instant" ) ) layer = INTEL_LAYER_INST;
	else if( !strcmp( name, "max instant" ) ) layer = INTEL_LAYER_MXIN;
	else if( !strcmp( name, "2x3" ) ) layer = INTEL_LAYER_2X3;
	else if( !strcmp( name, "2x4" ) ) layer = INTEL_LAYER_2X4;
	else if( !strcmp( name, "core power" ) ) layer = INTEL_LAYER_CORP;
	else if( !strcmp( name, "core amp" ) ) layer = INTEL_LAYER_CORA;
	else if( !strcmp( name, "core volt" ) ) layer = INTEL_LAYER_CORV;
	else if( !strcmp( name, "uncore power" ) ) layer = INTEL_LAYER_UNCP;
	else if( !strcmp( name, "uncore amp" ) ) layer = INTEL_LAYER_UNCA;
	else if( !strcmp( name, "uncore volt" ) ) layer = INTEL_LAYER_UNCV;
	else if( !strcmp( name, "memory power" ) ) layer = INTEL_LAYER_MEMP;
	else if( !strcmp( name, "memory amp" ) ) layer = INTEL_LAYER_MEMA;
	else if( !strcmp( name, "memory volt" ) ) layer = INTEL_LAYER_MEMV;
    else {
        fprintf( stderr, "Error: unknown layer specification %s\n", name );
    }
 
    DBGP( "Info: extracted open string (layer=%d)\n", layer );

    return layer;
}


static int micdev_gather(struct mic_device *mdh, layer_t layer, double *power)
{
	uint32_t pwr;
    struct mic_power_util_info *pinfo;
	mic_get_power_utilization_info(mdh, &pinfo);
	
	if (layer == INTEL_LAYER_WIN0) mic_get_total_power_readings_w0(pinfo, &pwr);	   	// watts	
	else if (layer == INTEL_LAYER_WIN1) mic_get_total_power_readings_w1(pinfo, &pwr);  	// watts
	else if (layer == INTEL_LAYER_PCIE) mic_get_pcie_power_readings(pinfo, &pwr);	   	// watts	
	else if (layer == INTEL_LAYER_INST) mic_get_inst_power_readings(pinfo, &pwr);  		// watts
	else if (layer == INTEL_LAYER_MXIN) mic_get_max_inst_power_readings(pinfo, &pwr);	// watts
	else if (layer == INTEL_LAYER_2X3) mic_get_c2x3_power_readings(pinfo, &pwr);    	// watts
	else if (layer == INTEL_LAYER_2X4) mic_get_c2x4_power_readings(pinfo, &pwr);    	// watts
	else if (layer == INTEL_LAYER_CORP) mic_get_vccp_power_readings(pinfo, &pwr);   	// watts
	else if (layer == INTEL_LAYER_CORA) mic_get_vccp_current_readings(pinfo, &pwr); 	// amps
	else if (layer == INTEL_LAYER_CORV) mic_get_vccp_voltage_readings(pinfo, &pwr); 	// volts
	else if (layer == INTEL_LAYER_UNCP) mic_get_vddg_power_readings(pinfo, &pwr);   	// watts
	else if (layer == INTEL_LAYER_UNCA) mic_get_vddg_current_readings(pinfo, &pwr); 	// amps
	else if (layer == INTEL_LAYER_UNCV) mic_get_vddg_voltage_readings(pinfo, &pwr); 	// volts
	else if (layer == INTEL_LAYER_MEMP) mic_get_vddq_power_readings(pinfo, &pwr);   	// watts
	else if (layer == INTEL_LAYER_MEMA) mic_get_vddq_current_readings(pinfo, &pwr); 	// amps
	else if (layer == INTEL_LAYER_MEMV) mic_get_vddq_voltage_readings(pinfo, &pwr); 	// volts
	
	(void)mic_free_power_utilization_info(pinfo);
	
	*power = (double)pwr / 1000000.0; // convert from micro-units (watts, amps, volts)

    return 0;
}

plugin_devops_t *pwr_micdev_init( const char *initstr )
{
    int i, ret;
    struct mic_devices_list *mdl;
	struct mic_pci_config *pcfg;
	struct mic_cores_info *cinfo;
	struct mic_power_limit *limit;
	
    plugin_devops_t *dev = malloc( sizeof(plugin_devops_t) );
    private_data_t* priv = dev->private_data = malloc( sizeof(private_data_t) );
	
    priv->numMics = 0;

	// Query number of connected MIC devices
    ret = mic_get_devices(&mdl);
    assert(ret == E_MIC_SUCCESS);

    ret = mic_get_ndevices(mdl, &priv->numMics);
	assert(ret == E_MIC_SUCCESS);
	
	priv->mics = malloc(priv->numMics * sizeof(pwr_micdev_t*));

    // for each MIC card connected
	for (i=0; i<priv->numMics; i++)
	{
		int card;
		uint16_t device_id;
		
		priv->mics[i] = malloc(sizeof(pwr_micdev_t));
		priv->mics[i]->mic=i;
		
		// Locate card and obtain handle
		ret = mic_get_device_at_index(mdl, i, &card);
		assert(ret == E_MIC_SUCCESS);

		ret = mic_open_device(&priv->mics[i]->mdh, card);
		assert(ret == E_MIC_SUCCESS);
		
		// Obtain pci config to determine device ID
		ret = mic_get_pci_config(priv->mics[i]->mdh, &pcfg);
		assert(ret == E_MIC_SUCCESS);
		
		ret = mic_get_device_id(pcfg, &device_id);
		assert(ret == E_MIC_SUCCESS);
		
		(void)mic_free_pci_config(pcfg); // release pci config
		
		if (device_id == KNCID)
			priv->mics[i]->device_id = "KNC";
		else if (device_id == KNLID)
			priv->mics[i]->device_id = "KNL";
		else
			priv->mics[i]->device_id = "Error";
		
		// Obtain core config to determine num cores and operating frequency
		ret = mic_get_cores_info(priv->mics[i]->mdh, &cinfo);
		assert(ret == E_MIC_SUCCESS);
		
		ret = mic_get_cores_count(cinfo, &priv->mics[i]->cores);
		assert(ret == E_MIC_SUCCESS);
		
		ret = mic_get_cores_frequency(cinfo, &priv->mics[i]->freq);
		assert(ret == E_MIC_SUCCESS);
		
		(void)mic_free_cores_info(cinfo); // release core config
		
		// Obtain power limit config to fill power_t
		ret = mic_get_power_limit(priv->mics[i]->mdh, &limit);
		assert(ret == E_MIC_SUCCESS);
		
		ret = mic_get_power_phys_limit(limit, &priv->mics[i]->power_specs.limitphysical);
		assert(ret == E_MIC_SUCCESS);
		
		ret = mic_get_power_hmrk(limit, &priv->mics[i]->power_specs.limithigh);
		assert(ret == E_MIC_SUCCESS);
		
		ret = mic_get_power_lmrk(limit, &priv->mics[i]->power_specs.limitlow);
		assert(ret == E_MIC_SUCCESS);
		
		ret = mic_get_time_window0(limit, &priv->mics[i]->power_specs.window0);
		assert(ret == E_MIC_SUCCESS);
		
		ret = mic_get_time_window1(limit, &priv->mics[i]->power_specs.window1);
		assert(ret == E_MIC_SUCCESS);
		
		(void)mic_free_power_limit(limit); // release power limit config
	}
	
	(void)mic_free_devices(mdl);

    DBGP("num Xeon Phi %d\n", priv->numMics );
    assert( priv->numMics > 0 );

    *dev = devops;
    dev->private_data = priv;
    return dev;
}

static void _pwr_micdev_fini( pwr_micdev_t* dev )
{
	(void)mic_close_device(dev->mdh);
}

int pwr_micdev_final( plugin_devops_t *dev )
{
    DBGP( "Info: PWR MIC device close\n" );

    private_data_t* priv = dev->private_data;  
    int i;
    for ( i = 0; i < priv->numMics; i++) {
        _pwr_micdev_fini( priv->mics[i]);
	free( priv->mics[i] );	
    } 
    free( dev->private_data );
    free( dev );

    return 0;
}

pwr_fd_t pwr_micdev_open( plugin_devops_t *dev, const char *openstr )
{
    private_data_t *info = (private_data_t*) dev->private_data;
    pwr_micfd_t *fd = malloc( sizeof(pwr_micfd_t) );

    int   rc;
    int   global_index;
    PWR_ObjType type;

    sscanf( openstr, "%d %d", &type, &global_index );
    DBGP("type=%d global_index=%d\n",type,global_index);

    switch ( type ) {
      case PWR_OBJ_SOCKET:
        fd->layers[0] = micdev_parse_open( "pci" );
        fd->layers[1] = micdev_parse_open( "2x3" );
		fd->layers[2] = micdev_parse_open( "2x4" );
        fd->numLayers = 3;
        break;
    }

    assert( global_index < info->numMics );

    fd->dev = info->mics[global_index ];

    return fd;
}

int pwr_micdev_close( pwr_fd_t fd )
{
    PWR_MICFD(fd)->dev = 0x0;
    free( fd );

    return 0;
}

int pwr_micdev_read( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len, PWR_Time *timestamp )
{
    int i;
    double power = 0.0;
	double time = 0.0;
#ifndef USE_SYSTIME
    struct timeval tv;
#endif

    DBGP( "Info: PWR MIC device read\n" );

    if( len != sizeof(double) ) {
        fprintf( stderr, "Error: value field size of %u incorrect, should be %ld\n", len, sizeof(double) );
        return -1;
    }

    for ( i = 0; i < PWR_MICFD(fd)->numLayers; i++ ) {
	double value = 0.0;
        if( micdev_gather( (PWR_MICFD(fd)->dev)->mdh,
                        PWR_MICFD(fd)->layers[i], &value) < 0 ) {
            fprintf( stderr, "Error: PWR MIC device gather failed\n" );
            return -1;
        }
	power += value;
    }

    switch( attr ) {
        case PWR_ATTR_POWER:
            *((double *)value) = power;
            break;
        default:
            fprintf( stderr, "Warning: unknown PWR reading attr requested\n" );
            break;
    }
#ifndef USE_SYSTIME
    gettimeofday( &tv, NULL );
    *timestamp = tv.tv_sec*1000000000ULL + tv.tv_usec*1000;
#else
    *timestamp = (unsigned int)time*1000000000ULL + 
                 (time-(unsigned int)time)*1000000000ULL;
#endif

    return 0;
}

int pwr_micdev_write( pwr_fd_t fd, PWR_AttrName attr, void *value, unsigned int len )
{
    return 0;
}

int pwr_micdev_readv( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, PWR_Time timestamp[], int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_micdev_read( fd, attrs[i], (double *)values+i, sizeof(double), timestamp+i );

    return 0;
}

int pwr_micdev_writev( pwr_fd_t fd, unsigned int arraysize,
    const PWR_AttrName attrs[], void *values, int status[] )
{
    unsigned int i;

    for( i = 0; i < arraysize; i++ )
        status[i] = pwr_micdev_write( fd, attrs[i], (double *)values+i, sizeof(double) );

    return 0;
}

int pwr_micdev_time( pwr_fd_t fd, PWR_Time *timestamp )
{
    double value;

    DBGP( "Info: PWR MIC device time\n" );

    return pwr_micdev_read( fd, PWR_ATTR_POWER, &value, sizeof(double), timestamp );
}

int pwr_micdev_clear( pwr_fd_t fd )
{
    return 0;
}

static plugin_dev_t dev = {
    .init   = pwr_micdev_init,
    .final  = pwr_micdev_final,
};

plugin_dev_t* getDev() {
    return &dev;
}

static int micdev_numObjs( )
{
    DBGP("\n");
    return 2;
}
static int micdev_readObjs(  int i, PWR_ObjType* ptr )
{
    DBGP("\n");
    ptr[0] = PWR_OBJ_SOCKET;
    return 0;
}

static int micdev_numAttrs( )
{
    DBGP("\n");
    return 1;
}

static int micdev_readAttrs( int i, PWR_AttrName* ptr )
{
    DBGP("\n");
    ptr[0] = PWR_ATTR_POWER;
    return 0;
}

static int micdev_getDevName(PWR_ObjType type, size_t len, char* buf )
{
    strncpy(buf,"MIC", len );
    DBGP("type=%d name=`%s`\n",type,buf);
    return 0;
}

static int micdev_getDevOpenStr(PWR_ObjType type,
                        int global_index, size_t len, char* buf )
{
    snprintf( buf, len, "%d %d", type, global_index);
    DBGP("type=%d global_index=%d str=`%s`\n",type,global_index,buf);
    return 0;
}

static int micdev_getDevInitStr( const char* name,
                        size_t len, char* buf )
{
    snprintf(buf,len,"");
    DBGP("dev=`%s` str=`%s`\n",name, buf );
    return 0;
}

static int micdev_getPluginName( size_t len, char* buf )
{
    snprintf(buf,len,"micmgmt");
    return 0;
}


static plugin_meta_t meta = {
    .numObjs = micdev_numObjs,
    .numAttrs = micdev_numAttrs,
    .readObjs = micdev_readObjs,
    .readAttrs = micdev_readAttrs,
    .getDevName = micdev_getDevName,
    .getDevOpenStr = micdev_getDevOpenStr,
    .getDevInitStr = micdev_getDevInitStr,
	.getPluginName = micdev_getPluginName,
};

plugin_meta_t* getMeta() {
    return &meta;
}

