#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "plugins/mchw_pidev.h"
#include "plugins/mchw_rapldev.h"
#include "plugins/mchw_xtpmdev.h"

int main( int argc, char* argv[] )
{
    plugin_dev_t pwr_dev[] = {
        { mchw_pidev_init, mchw_pidev_final, mchw_pidev_open, mchw_pidev_close,
          mchw_pidev_read, mchw_pidev_write, mchw_pidev_readv, mchw_pidev_writev,
          mchw_pidev_time, mchw_pidev_clear },
        { mchw_rapldev_init, mchw_rapldev_final, mchw_rapldev_open, mchw_rapldev_close,
          mchw_rapldev_read, mchw_rapldev_write, mchw_rapldev_readv, mchw_rapldev_writev,
          mchw_rapldev_time, mchw_rapldev_clear },
        { mchw_xtpmdev_init, mchw_xtpmdev_final, mchw_xtpmdev_open, mchw_xtpmdev_close,
          mchw_xtpmdev_read, mchw_xtpmdev_write, mchw_xtpmdev_readv, mchw_xtpmdev_writev,
          mchw_xtpmdev_time, mchw_xtpmdev_clear }
    };
    char initstr[][80] = {
        "10.54.21.97:20201", 
        "0:0",
        ""
    };
    char openstr[][80] = {
        "1", 
        "",
        ""
    };

    pwr_dev_t dev;
    pwr_fd_t fd;
    unsigned int dsize = sizeof(pwr_dev)/sizeof(plugin_dev_t);

    PWR_AttrName rtype[] = { 
        PWR_ATTR_MIN_POWER,
        PWR_ATTR_MAX_POWER,
        PWR_ATTR_POWER,
        PWR_ATTR_ENERGY
    };
    double rval[] = { 0.0, 0.0 };

    PWR_AttrName wtype[] = {
        PWR_ATTR_MIN_PCAP,
        PWR_ATTR_MAX_PCAP
    };
    double wval[] = { 20.0, 120.0 };

    unsigned int rsize = sizeof(rval)/sizeof(double);
    unsigned int wsize = sizeof(wval)/sizeof(double);

    PWR_Time rtime[rsize];
    int rstat[rsize], wstat[wsize];

    unsigned int i, j;

    for( i = 0; i < dsize; i++ ) {
        if( (dev=pwr_dev[i].init( initstr[i] )) == 0x0 ) {
            printf( "Error initializing power device #%u\n", i );
            return -1;
        }
 
        if( (fd=pwr_dev[i].open( dev, openstr[i] )) == 0x0 ) {
            printf( "Error opening power device #%u\n", i );
            return -1;
        }

        if( pwr_dev[i].readv( fd, rsize, rtype, rval, rtime, rstat ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
        else {
            for( j = 0; j < rsize; j++ ) {
                printf( "Reading %u: %f\n", rtype[j], rval[j] );
            }
        }
 
        if( pwr_dev[i].writev( fd, wsize, wtype, wval, wstat ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
        else {
            for( j = 0; j < wsize; j++ ) {
                printf( "Setting %u: %f\n", wtype[j], wval[j] );
            }
        }
 
        if( pwr_dev[i].close( fd ) < 0 ) {
            printf( "Error closing power device #%u\n", i );
            return -1;
        }

        if( pwr_dev[i].final( dev ) < 0 ) {
            printf( "Error finalizing power device #%u\n", i );
            return -1;
        }
    }
 
    return 0;
}

