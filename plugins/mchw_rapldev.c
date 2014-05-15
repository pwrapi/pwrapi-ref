#include "mchw_rapldev.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <types.h>
#include <math.h>

#define CPU_MODEL_SANDY    42
#define CPU_MODEL_SANDY_EP 45
#define CPU_MODEL_IVY      58
#define CPU_MODEL_IVY_EP   62
#define CPU_MODEL_HASWELL  60

#define MSR_RAPL_POWER_UNIT    0x606

#define MSR_POWER_LIMIT        0x610
#define MSR_ENERGY_STATUS      0x611
#define MSR_PERF_STATUS        0x613
#define MSR_POWER_INFO         0x614

#define MSR_PP0_POWER_LIMIT    0x638
#define MSR_PP0_ENERGY_STATUS  0x639
#define MSR_PP0_POLICY         0x63a
#define MSR_PP0_PERF_STATUS    0x63b

#define MSR_PP1_POWER_LIMIT    0x640
#define MSR_PP1_ENERGY_STATUS  0x641
#define MSR_PP1_POLICY         0x642

#define MSR_DRAM_POWER_LIMIT   0x618
#define MSR_DRAM_ENERGY_STATUS 0x619
#define MSR_DRAM_PERF_STATUS   0x61b
#define MSR_DRAM_POWER_INFO    0x61c

#define POWER_UNITS_SHIFT    1
#define ENERGY_UNITS_SHIFT   8
#define TIME_UNITS_SHIFT    16

#define POWER_UNITS_MASK       0xf
#define ENERGY_UNITS_MASK      0x1f
#define TIME_UNITS_MASK        0xf

#define THERMAL_POWER_SHIFT  0
#define MINIMUM_POWER_SHIFT 16
#define MAXIMUM_POWER_SHIFT 32
#define WINDOW_POWER_SHIFT  48

#define THERMAL_POWER_MASK     0x7fff
#define MINIMUM_POWER_MASK     0x7fff
#define MAXIMUM_POWER_MASK     0x7fff
#define WINDOW_POWER_MASK      0x7fff

#define POWER1_LIMIT_SHIFT   0
#define WINDOW1_LIMIT_SHIFT 17
#define ENABLED1_LIMIT_BIT  15
#define CLAMPED1_LIMIT_BIT  16
#define POWER2_LIMIT_SHIFT  32
#define WINDOW2_LIMIT_SHIFT 49
#define ENABLED2_LIMIT_BIT  47
#define CLAMPED2_LIMIT_BIT  48

#define POWER1_LIMIT_MASK      0x7fff
#define WINDOW1_LIMIT_MASK     0x007f
#define POWER2_LIMIT_MASK      0x7fff
#define WINDOW2_LIMIT_MASK     0x007f

#define MSR(X,Y,Z) (X>>Y&Z)
#define MSR_BIT(X,Y) (X&(1LL<<Y))

typedef struct {
    int fd;
    int cpu_model;

    struct {
    	double power;  /* power units in W */
    	double energy; /* energy units in J */
    	double time;   /* time units in s */
    } units;

    struct {
        double thermal; /* thermal specification */
        double minimum; /* minimum power */
        double maximum; /* maximum power */
        double window;  /* maximum time window */
    } power;

    struct {
        double power1;  /* power limit 1 */
        double window1; /* time window 1 */
        unsigned short enabled1; /* limit 1 enabled */
        unsigned short clamped1; /* limit 1 clamped */
        double power2;  /* power limit 2 */
        double window2; /* time window 2 */
        unsigned short enabled2; /* limit 2 enabled */
        unsigned short clamped2; /* limit 2 clamped */
    } limit;
} mchw_rapldev_t;

#define MCHW_RAPLDEV(X) ((mchw_rapldev_t *)(X))

static int rapldev_parse( char *initstr, int *core )
{
    if( initstr ) {
        *core = atoi(initstr);
    }
 
    return 0;
}

static int rapldev_identify( int *cpu_model )
{
    FILE *file;
    char *str = 0x0;
    char cpuinfo[80] = "";
    int retval = 0;

    int family;
    char vendor[80] = "";

    if( (file=fopen( "/proc/cpuinfo", "r" )) == 0x0 ) {
        printf( "Error: RAPL cpuinfo open failed\n" );
        return -1;
    }

    *cpu_model = -1;
    while( (str=fgets( cpuinfo, 80, file )) != 0x0 ) {
        if( strncmp( str, "vendor_id", 8) == 0 ) {
            sscanf( str, "%*s%*s%s", vendor );
            if( strncmp( str, "GenuineIntel", 12 ) != 0 ) {
                printf( "Warning: %s, only Intel supported\n", vendor );
                retval = -1;
            }
        }
        else if( strncmp( str, "cpu_family", 10) == 0 ) {
            sscanf( str, "%*s%*s%*s%d", &family );
            if( family != 6 ) {
                printf( "Warning: Unsupported CPU family %d\n", family );
                retval = -1;
            }
        }
        else if( strncmp( str, "model", 5) == 0 ) {
            sscanf( str, "%*s%*s%d", cpu_model );

            switch( *cpu_model ) {
                case CPU_MODEL_SANDY:
                case CPU_MODEL_SANDY_EP:
                case CPU_MODEL_IVY:
                case CPU_MODEL_IVY_EP:
                case CPU_MODEL_HASWELL:
                    break;
                default:
                    printf( "Warning: Unsupported model %d\n", *cpu_model );
                    retval = -1;
                    break;
            }
        }
    }

    fclose( file );
    return retval;
}

static int rapldev_read( int fd, int offset, long long *msr )
{
    uint64_t value;
    int size = sizeof(uint64_t);

    if( pread( fd, &value, size, offset ) != size ) {
        printf( "Error: RAPL read failed\n" );
        return -1;
    }
 
    *msr = (long long)value;
    return 0;
}

int mchw_rapldev_init( mchw_dev_t *dev, char *initstr )
{
    char file[80] = "";
    int core = 0;
    long long msr;
    *dev = malloc( sizeof(mchw_rapldev_t) );

    if( rapldev_parse( initstr, &core ) < 0 ) {
        printf( "Error: MCHW RAPL device initialization string %s invalid\n", initstr );
        return -1;
    }

    if( rapldev_identify( &(MCHW_RAPLDEV(*dev)->cpu_model) ) < 0 ) {
        printf( "Error: MCHW RAPL device model identification failed\n" );
        return -1;
    }

    sprintf( file, "/dev/cpu/%d/msr", core );
    if( (MCHW_RAPLDEV(*dev)->fd=open( file, O_RDONLY )) < 0 ) {
        printf( "Error: MCHW RAPL device open failed\n" );
        return -1;
    }

    if( rapldev_read( MCHW_RAPLDEV(*dev)->fd, MSR_RAPL_POWER_UNIT, &msr ) < 0 ) {
        printf( "Error: MCHW RAPL device read failed\n" );
        return -1;
    }
    MCHW_RAPLDEV(*dev)->units.power = 
        pow( 0.5, (double)(MSR(msr, POWER_UNITS_SHIFT, POWER_UNITS_MASK)) );
    MCHW_RAPLDEV(*dev)->units.energy = 
        pow( 0.5, (double)(MSR(msr, ENERGY_UNITS_SHIFT, ENERGY_UNITS_MASK)) );
    MCHW_RAPLDEV(*dev)->units.time =
        pow( 0.5, (double)(MSR(msr, TIME_UNITS_SHIFT, TIME_UNITS_MASK)) );

    if( rapldev_read( MCHW_RAPLDEV(*dev)->fd, MSR_POWER_INFO, &msr ) < 0 ) {
        printf( "Error: MCHW RAPL device read failed\n" );
        return -1;
    }
    MCHW_RAPLDEV(*dev)->power.thermal =
        (double)(MSR(msr, THERMAL_POWER_SHIFT, THERMAL_POWER_MASK));
    MCHW_RAPLDEV(*dev)->power.minimum =
        (double)(MSR(msr, MINIMUM_POWER_SHIFT, MINIMUM_POWER_MASK));
    MCHW_RAPLDEV(*dev)->power.maximum =
        (double)(MSR(msr, MAXIMUM_POWER_SHIFT, MAXIMUM_POWER_MASK));
    MCHW_RAPLDEV(*dev)->power.window =
        (double)(MSR(msr, WINDOW_POWER_SHIFT, WINDOW_POWER_MASK));

    if( rapldev_read( MCHW_RAPLDEV(*dev)->fd, MSR_POWER_LIMIT, &msr ) < 0 ) {
        printf( "Error: MCHW RAPL device read failed\n" );
        return -1;
    }
    MCHW_RAPLDEV(*dev)->limit.power1 =
        (double)(MSR(msr, POWER1_LIMIT_SHIFT, POWER1_LIMIT_MASK));
    MCHW_RAPLDEV(*dev)->limit.window1 =
        (double)(MSR(msr, WINDOW1_LIMIT_SHIFT, WINDOW1_LIMIT_MASK));
    MCHW_RAPLDEV(*dev)->limit.enabled1 =
        (double)(MSR_BIT(msr, ENABLED1_LIMIT_BIT));
    MCHW_RAPLDEV(*dev)->limit.clamped1 =
        (double)(MSR_BIT(msr, CLAMPED1_LIMIT_BIT));
    MCHW_RAPLDEV(*dev)->limit.power2 =
        (double)(MSR(msr, POWER2_LIMIT_SHIFT, POWER2_LIMIT_MASK));
    MCHW_RAPLDEV(*dev)->limit.window2 =
        (double)(MSR(msr, WINDOW2_LIMIT_SHIFT, WINDOW2_LIMIT_MASK));
    MCHW_RAPLDEV(*dev)->limit.enabled2 =
        (double)(MSR_BIT(msr, ENABLED2_LIMIT_BIT));
    MCHW_RAPLDEV(*dev)->limit.clamped2 =
        (double)(MSR_BIT(msr, CLAMPED2_LIMIT_BIT));
 
    return 0;
}

int mchw_rapldev_final( mchw_dev_t *dev )
{
    close( MCHW_RAPLDEV(*dev)->fd );

    free( *dev );
    *dev = 0x0;

    return 0;
}

int mchw_rapldev_read( mchw_dev_t dev, unsigned int arraysize,
	mchw_read_type_t type[], float reading[], unsigned long long *timestamp )
{
    return 0;
}

int mchw_rapldev_time( mchw_dev_t dev, unsigned long long *time )
{
    return 0;
}

