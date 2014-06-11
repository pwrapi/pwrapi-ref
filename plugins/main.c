#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "plugins/mchw_pidev.h"
#include "plugins/mchw_rapldev.h"
#include "plugins/mchw_xtpmdev.h"

int main( int argc, char* argv[] )
{
    plugin_dev_t pwr_dev[] = {
        { mchw_pidev_open, mchw_pidev_close, mchw_pidev_read, mchw_pidev_write,
          mchw_pidev_readv, mchw_pidev_writev, mchw_pidev_time, mchw_pidev_clear },
        { mchw_rapldev_open, mchw_rapldev_close, mchw_rapldev_read, mchw_rapldev_write,
          mchw_rapldev_readv, mchw_rapldev_writev, mchw_rapldev_time, mchw_rapldev_clear },
        { mchw_xtpmdev_open, mchw_xtpmdev_close, mchw_xtpmdev_read, mchw_xtpmdev_write,
          mchw_xtpmdev_readv, mchw_xtpmdev_writev, mchw_xtpmdev_time, mchw_xtpmdev_clear }
    };
    char initstr[][80] = {
        "10.54.21.97:20201:1", 
        "0:0",
        ""
    };

    pwr_dev_t dev;
    unsigned int dsize = sizeof(pwr_dev)/sizeof(plugin_dev_t);

    PWR_AttrName rtype[] = { 
        PWR_ATTR_MIN_POWER,
        PWR_ATTR_MAX_POWER,
        PWR_ATTR_POWER,
        PWR_ATTR_ENERGY
    };
    float rval[] = { 0.0, 0.0 };

    PWR_AttrName wtype[] = {
        PWR_ATTR_MIN_PCAP,
        PWR_ATTR_MAX_PCAP
    };
    float wval[] = { 20.0, 120.0 };

    unsigned int rsize = sizeof(rval)/sizeof(float);
    unsigned int wsize = sizeof(wval)/sizeof(float);

    PWR_Time rtime[rsize];
    int rstat[rsize], wstat[wsize];

    unsigned int i, j;

    for( i = 0; i < dsize; i++ ) {
        if( (dev=pwr_dev[i].open( initstr[i] )) == 0x0 ) {
            printf( "Error opening power device #%u\n", i );
            return -1;
        }
 
        if( pwr_dev[i].readv( dev, rsize, rtype, rval, rtime, rstat ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
        else {
            for( j = 0; j < rsize; j++ ) {
                printf( "Reading %u: %f\n", rtype[j], rval[j] );
            }
        }
 
        if( pwr_dev[i].writev( dev, wsize, wtype, wval, wstat ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
        else {
            for( j = 0; j < wsize; j++ ) {
                printf( "Setting %u: %f\n", wtype[j], wval[j] );
            }
        }
 
        if( pwr_dev[i].close( dev ) < 0 ) {
            printf( "Error closing power device #%u\n", i );
            return -1;
        }
    }
 
    return 0;
}

