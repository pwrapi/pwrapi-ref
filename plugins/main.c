#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "plugins/mchw_pidev.h"
#include "plugins/mchw_rapldev.h"
#include "plugins/mchw_xtpmdev.h"

int main( int argc, char* argv[] )
{
    plugin_dev_t pwr_dev[] = {
        { mchw_pidev_open, mchw_pidev_close, mchw_pidev_read, mchw_pidev_write, mchw_pidev_time },
        { mchw_rapldev_open, mchw_rapldev_close, mchw_rapldev_read, mchw_rapldev_write, mchw_rapldev_time },
        { mchw_xtpmdev_open, mchw_xtpmdev_close, mchw_xtpmdev_read, mchw_xtpmdev_write, mchw_xtpmdev_time }
    };
    char initstr[][80] = {
        "10.54.21.97:20201:1", 
        "0:0",
        ""
    };

    pwr_dev_t dev;
    unsigned int dsize = sizeof(pwr_dev)/sizeof(plugin_dev_t);

    PWR_Value rval[] = {
        { PWR_ATTR_MIN_POWER, malloc(sizeof(float)), sizeof(float), 0 },
        { PWR_ATTR_MAX_POWER, malloc(sizeof(float)), sizeof(float), 0 },
        { PWR_ATTR_POWER, malloc(sizeof(float)), sizeof(float), 0 },
        { PWR_ATTR_ENERGY, malloc(sizeof(float)), sizeof(float), 0 }
    };
    PWR_Value wval[] = {
        { PWR_ATTR_MIN_PCAP, malloc(sizeof(float)), sizeof(float), 0 },
        { PWR_ATTR_MAX_PCAP, malloc(sizeof(float)), sizeof(float), 0 }
    };
    unsigned int rsize = sizeof(rval)/sizeof(PWR_Value);
    unsigned int wsize = sizeof(wval)/sizeof(PWR_Value);
    unsigned int i, j;

    *((float *)(wval[0].ptr)) = 20;
    *((float *)(wval[1].ptr)) = 120;

    for( i = 0; i < dsize; i++ ) {
        if( (dev=pwr_dev[i].open( initstr[i] )) == 0x0 ) {
            printf( "Error opening power device #%u\n", i );
            return -1;
        }
 
        if( pwr_dev[i].read( dev, rsize, rval ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
        else {
            for( j = 0; j < rsize; j++ ) {
                printf( "Reading %u: %f\n", rval[j].type, *((float *)(rval[j].ptr)) );
            }
        }
 
        if( pwr_dev[i].write( dev, wsize, wval ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
        else {
            for( j = 0; j < wsize; j++ ) {
                printf( "Setting %u: %f\n", wval[j].type, *((float *)(wval[j].ptr)) );
            }
        }
 
        if( pwr_dev[i].close( dev ) < 0 ) {
            printf( "Error closing power device #%u\n", i );
            return -1;
        }
    }
 
    return 0;
}

