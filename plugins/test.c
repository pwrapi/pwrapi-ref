/* 
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pwr_rapldev.h"
#include "pwr_xtpmdev.h"

int main( int argc, char* argv[] )
{
    plugin_dev_t pwr_dev[] = {
        { pwr_rapldev_init, pwr_rapldev_final },
        { pwr_xtpmdev_init, pwr_xtpmdev_final }
    };

    plugin_devops_t pwr_devops[] = {
        { pwr_rapldev_open, pwr_rapldev_close,
          pwr_rapldev_read, pwr_rapldev_write, pwr_rapldev_readv, pwr_rapldev_writev,
          pwr_rapldev_time, pwr_rapldev_clear },
        { pwr_xtpmdev_open, pwr_xtpmdev_close,
          pwr_xtpmdev_read, pwr_xtpmdev_write, pwr_xtpmdev_readv, pwr_xtpmdev_writev,
          pwr_xtpmdev_time, pwr_xtpmdev_clear }
    };
    char initstr[][80] = {
        "0:0",
        ""
    };
    char openstr[][80] = {
        "1", 
        "",
        ""
    };

    plugin_devops_t *devops;
    pwr_fd_t fd;
    unsigned int dsize = sizeof(pwr_devops)/sizeof(plugin_devops_t);

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
        if( (devops=pwr_dev[i].init( initstr[i] )) == 0x0 ) {
            printf( "Error initializing power device #%u\n", i );
            return -1;
        }
 
        if( (fd=pwr_devops[i].open( devops, openstr[i] )) == 0x0 ) {
            printf( "Error opening power device #%u\n", i );
            return -1;
        }

        if( pwr_devops[i].readv( fd, rsize, rtype, rval, rtime, rstat ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
        else {
            for( j = 0; j < rsize; j++ ) {
                printf( "Reading %u: %f\n", rtype[j], rval[j] );
            }
        }
 
        if( pwr_devops[i].writev( fd, wsize, wtype, wval, wstat ) < 0 ) {
            printf( "Error reading from power device #%u\n", i );
            return -1;
        }
        else {
            for( j = 0; j < wsize; j++ ) {
                printf( "Setting %u: %f\n", wtype[j], wval[j] );
            }
        }
 
        if( pwr_devops[i].close( fd ) < 0 ) {
            printf( "Error closing power device #%u\n", i );
            return -1;
        }

        if( pwr_dev[i].final( devops ) < 0 ) {
            printf( "Error finalizing power device #%u\n", i );
            return -1;
        }
    }
 
    return 0;
}

