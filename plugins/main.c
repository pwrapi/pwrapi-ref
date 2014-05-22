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

    PWR_AttrType rtype[] = { PWR_ATTR_MIN_POWER, PWR_ATTR_MAX_POWER, PWR_ATTR_POWER, PWR_ATTR_ENERGY };
    PWR_AttrType wtype[] = { PWR_ATTR_MIN_POWER_CAP, PWR_ATTR_MAX_POWER_CAP };
    unsigned int rsize = sizeof(rtype)/sizeof(PWR_AttrType);
    unsigned int wsize = sizeof(wtype)/sizeof(PWR_AttrType);

    float reading[rsize];
    float setting[wsize];
    unsigned long long time;

    unsigned int i, j;

    setting[0] = 20; setting[1] = 120;

    for( i = 0; i < dsize; i++ ) {
        if( pwr_dev[i].open( &dev, initstr[i] ) < 0 ) {
            printf( "Error opening power device #%u\n", i );
            return -1;
        }
 
        if( pwr_dev[i].read( dev, rsize, rtype, reading, &time ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
        else {
            for( j = 0; j < rsize; j++ ) {
                printf( "Reading %u: %f\n", rtype[j], reading[j] );
            }
        }
 
        if( pwr_dev[i].write( dev, wsize, wtype, setting, &time ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
        else {
            for( j = 0; j < wsize; j++ ) {
                printf( "Setting %u: %f\n", wtype[j], setting[j] );
            }
        }
 
        if( pwr_dev[i].close( &dev ) < 0 ) {
            printf( "Error closing power device #%u\n", i );
            return -1;
        }
    }
 
    return 0;
}

