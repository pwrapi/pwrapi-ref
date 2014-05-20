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
    char *initstr[] = { "10.54.21.10:20201:1", "0:0", "" };

    pwr_dev_t dev;
    const unsigned int ARRAY_MAX_SIZE = 10;

    PWR_AttrType rtype[] = { PWR_ATTR_MIN_POWER, PWR_ATTR_MAX_POWER, PWR_ATTR_POWER, PWR_ATTR_ENERGY };
    PWR_AttrType wtype[] = { PWR_ATTR_MIN_POWER_CAP, PWR_ATTR_MAX_POWER_CAP };
    float reading[ARRAY_MAX_SIZE], setting[ARRAY_MAX_SIZE];
    unsigned long long time;
    unsigned int i;

    for( i = 0; i < 3; i++ ) {
        if( pwr_dev[i].open( &dev, initstr[i] ) < 0 ) {
            printf( "Error opening power device #%u\n", i );
            return -1;
        }
 
        if( pwr_dev[i].read( dev, sizeof(rtype), rtype, reading, &time ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
 
        if( pwr_dev[i].write( dev, sizeof(wtype), wtype, setting, &time ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
 
        if( pwr_dev[i].close( &dev ) < 0 ) {
            printf( "Error closing power device #%u\n", i );
            return -1;
        }
    }
 
    return 0;
}

