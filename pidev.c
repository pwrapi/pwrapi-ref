/* getRawPower9.5.c   (version 1)
 * For use with Carrier Board 10016423 Rev E8 (= Rev A)
 * Sandia Q195871 configuration 
 */

#include "pidev.h"

#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>	// SPI_MODE_0,

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define SPI_BITS         (8)
#define SPI_DELAY        (0)
#define SPI_SPEED        (1000000)
#define MEASURE_AMPS     1
#define MEASURE_VOLTS    0

// Offset = .1*Vdd
// Gain = 185mV/1000mA @ 5V Vdd
// Raw unit = Vdd/1024
// Amps = ((raw * Vdd/1024) - Offset) / Gain
// mAmps = ((raw * 5000/1024) - 5000/10) / (185/1000)
// mAmps = ((raw*10) - 1024) * 5000/(1024*10) * (1000/185)
// mAmps = (raw*10 - 1024)*500000/(1024*185)

#define SAMPLE2MAMPS(x)    (((x)*10-1024)*(500000.0/(180*1024)))

// Vref = 4.096V
// Raw unit = 4.096/65536 = 1/16 mV
// Voltage divider R1 over R2
// mVolts = raw * ((R1+R2)/R2) /16
#define SAMPLE_TO_12VOLTS(Vsamp)    ((Vsamp)*535/(133*16))  // 40.2k/13.3k

#define SAMPLE_TO_5VOLTS(Vsamp)     ((Vsamp)*414/(249*16))  // 16.5k/24.9k

#define SAMPLE_TO_3_3VOLTS(Vsamp)   ((Vsamp)*121/(110*16))  // 11k/110k

static uint16_t spiTransfer(int fd, int port);
static uint16_t spiVTransfer(int fd, int port);
static uint16_t ainTransfer(int fd, int port);
static void openSPI( int dev );
static void openAIN( int dev );
static void closeSPI( int dev );
static void closeAIN( int dev );

/* 
 *  There are 3 spi chip selects available: 
 *  CS0 on spiBus 1 (spidev1.0)  (shows as SPI0_CS0 on the schematic)
 *  CS0 on spiBus 2 (spidev2.0)  (shows as SPI1_CS0 on the schematic)
 *  CS1 on spiBus 2 (spidev2.1)  (shows as SPI1_CS1 on the schematic)
 */
struct {
   int  fd ;             // FD assigned to an opened FD
   const char *  file ;  // Name of file to be opened
   uint16_t  (*xfer)(int fd, int port) ;  // Transfer function to read
   void  (*open)( int dev );  // Open function
   void  (*close)( int dev );  // Close function
} devList[] = {
        { -1, "/dev/spidev1.0", spiTransfer, openSPI, closeSPI },
        { -1, "/dev/spidev2.0", spiVTransfer, openSPI, closeSPI },
        { -1, "/dev/spidev2.1", spiTransfer, openSPI, closeSPI },
        { -1, "/sys/devices/platform/omap/tsc/ain1", ainTransfer, openAIN, closeAIN },
        { -1, "/sys/devices/platform/omap/tsc/ain2", ainTransfer, openAIN, closeAIN },
        { -1, "/sys/devices/platform/omap/tsc/ain3", ainTransfer, openAIN, closeAIN },
        { -1, "/sys/devices/platform/omap/tsc/ain4", ainTransfer, openAIN, closeAIN },
        { -1, "/sys/devices/platform/omap/tsc/ain5", ainTransfer, openAIN, closeAIN },
        { -1, "/sys/devices/platform/omap/tsc/ain6", ainTransfer, openAIN, closeAIN },
        { -1, "/sys/devices/platform/omap/tsc/ain7", ainTransfer, openAIN, closeAIN }
    }; ;

typedef struct portConfig {
    int         ampsADC_chNum;
    int         voltsADC_chNum;
    int         AmpSpiDevNum;
    int         VoltSpiDevNum;
} portConfig_t;

/* 
 * Port numbers in the comments below are the index into this table.
 * Amps chNum is MPC3008 channel measuring this port's amp or volt signal.
 */
static portConfig_t  portConfig[] = {
/*                 Amps        Volts
     Amps  Volts   SPI device  SPI device
     chNum chNum   Number      Number
*/
    {7,    7,      2,          2},       // port dummy 
    {0,    0,      0,          1},       // port 1 
    {1,    1,      0,          1},       // port 2
    {2,    2,      0,          1},       // port 3
    {3,    3,      0,          1},       // port 4
    {4,    4,      0,          1},       // port 5
    {5,    5,      0,          1},       // port 6
    {6,    6,      0,          1},       // port 7
    {7,    7,      0,          1},       // port 8
    {0,    1,      2,          3},       // port 9   ain<VchNum>
    {1,    2,      2,          4},       // port 10
    {2,    3,      2,          5},       // port 11
    {3,    4,      2,          6},       // port 12
    {4,    5,      2,          7},       // port 13
    {5,    6,      2,          8},       // port 14
    {6,    7,      2,          9},       // port 15
};  

typedef enum portVolt {
    VOLT_UNKNOWN = -1,
    VOLT_SCALE = 0,
    VOLT_3_3 = 1,
    VOLT_5 = 2,
    VOLT_12 = 3
} portVolt_t;

typedef struct portMap {
    int portNumber;
    portVolt_t portVoltage;
} portMap_t;

static portMap_t portMap[] = {
    {  0, VOLT_UNKNOWN },
    {  1, VOLT_UNKNOWN },
    {  2, VOLT_UNKNOWN },
    {  3, VOLT_UNKNOWN },
    {  4, VOLT_UNKNOWN },
    {  5, VOLT_UNKNOWN },
    {  6, VOLT_UNKNOWN },
    {  7, VOLT_UNKNOWN },
    {  8, VOLT_UNKNOWN },
    {  9, VOLT_UNKNOWN },
    { 10, VOLT_UNKNOWN },
    { 11, VOLT_UNKNOWN },
    { 12, VOLT_UNKNOWN },
    { 13, VOLT_UNKNOWN },
    { 14, VOLT_UNKNOWN },
    { 15, VOLT_UNKNOWN }
};

static void configPortMap()
{
    FILE *fd;
    int i;

    fd = fopen( ".config", "r" );
    if( fd == NULL ) {
        printf( "Missing configuration file\n" );
        exit( 1 );
    }

    for( i = 0; i < 16; i++ )
        if( fscanf( fd, "%d", &(portMap[i].portVoltage) ) != 1) {
            printf( "Failed to read input configuration for port %d/", i );
            exit( 1 );
        }

    fclose( fd );
}

/***********************************************************/
static uint16_t spiTransfer(int fd, int port)
{
    int         result;
    uint16_t    retVal; 
    uint8_t     tx[] = { 0x01, 0x00, 0x00, };
    uint8_t     rx[] = { 0,    0,    0,    };
    int         txLen = 3;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = txLen, 
        .delay_usecs = SPI_DELAY,
        .speed_hz = SPI_SPEED,
        .bits_per_word = SPI_BITS,
    };

    tx[1] = 0x80 | ((port & 0x7) << 4) ;

    errno = 0 ;
    result = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    if (result < 1) {
        error( 0, errno, "In spiTransfer, ioctl() returned 0x%x\n", result);
        retVal = 0 ;
    } else {
        // mask sample bits received from MPC3008
        retVal = (((rx[1] & 3) << 8) | rx[2]);
    }
    return(retVal);
}   // end spiTransfer()


/***********************************************************/
static uint16_t spiVTransfer(int fd, int port)
{
    uint16_t retVal ;

    // Normal read
    retVal = spiTransfer( fd, port );

    // Scale to 16bit fraction of 4.096 volts
    return ( retVal * 64 );
}

/***********************************************************/
static uint16_t ainTransfer(int fd, int port)
{
    int  i ;
    char  buf[16] ;
    uint8_t  actual ;
    int  val, sum, min, max ;
    uint16_t  retVal; 

    lseek( fd, 0, SEEK_SET );
    actual = read( fd, buf, sizeof(buf) -1 );
    buf[actual] = '\0' ;
    min = val = atoi( buf );
    max = 0 ;
    
    sum = 0 ; 
    for( i = 17 ; i ; --i ) {
        lseek( fd, 0, SEEK_SET );
        actual = read( fd, buf, sizeof(buf) -1 );
        if( actual > 0 ) {
            buf[actual] = '\0' ;
            val = atoi( buf );
        };
        if( val < min ) {
            sum += min ;
            min = val ;
        } else if( val > max ) {
            sum += max ;
            max = val ;
        } else {
            sum += val ;
        };
    };

    // raw * 1800mV/4096 = mV
    // mV * 218/118 = mV reverse 11.8k pulldown
    // mV * 16 = 16bit fraction of 4.096V
    //  *16 = 16 summed samples
    retVal = sum *((1800.0/4096)*(218.0/118));

    // With 10k pulldown, it would be
    // retVal = sum *((1800.0/4096)*(20.0/10)*2)
    // retVal = sum *(1800.0/1024)

    return retVal ;
}


/***********************************************************/
static void openSPI( int dev )
{
    int  fd ;
    int  retVal, setting ;

    // Make sure it's not already open?
    if( devList[dev].fd >= 0 ) {
        return ;
    };

    // Open SPI device
    fd = open(devList[dev].file, O_RDWR);
    if (fd < 0) {
        error( 1, errno, "Error opening %s read/write", devList[dev].file );
    };

    // INITIALIZE SPI:
    setting = SPI_MODE_0 ;
    retVal = ioctl(fd, SPI_IOC_WR_MODE, &setting);
    if (retVal == -1) {
        error( 1, errno, "Can't set %s mode to 0", devList[dev].file );
    };

    setting = SPI_BITS ;
    retVal = ioctl( fd, SPI_IOC_WR_BITS_PER_WORD, &setting );
    if (retVal == -1) {
        error( 1, errno, "Can't set %s bits per word to %d", devList[dev].file, SPI_BITS );
    };

    setting = SPI_SPEED;
    retVal = ioctl( fd, SPI_IOC_WR_MAX_SPEED_HZ, &setting );
    if (retVal == -1) {
        error( 1, errno, "Can't set %s max speed to %d", devList[dev].file, SPI_SPEED );
    };

    // Throw away one conversion to reset charge buckets
    // See datasheet errata for MCP3008
    spiTransfer(fd, 0);

    // Get installation dependent port mapping
    configPortMap();

    // Completed, save it
    devList[dev].fd = fd ;

}

/***********************************************************/
static void openAIN( int dev )
{
    int  fd ;

    // Make sure it's not already open?
    if( devList[dev].fd >= 0 ) {
        return ;
    };

    // Open AIN device
    fd = open(devList[dev].file, O_RDONLY);
    if (fd < 0) {
        error( 1, errno, "Error opening %s read only", devList[dev].file );
    };

    // Get installation dependent port mapping
    configPortMap();

    // Completed, save it
    devList[dev].fd = fd ;
}

static void closeSPI( int dev ) {

    if( devList[dev].fd >= 0 ) {
        close( devList[dev].fd );
    }
}

static void closeAIN( int dev ) {

    if( devList[dev].fd >= 0 ) {
        close( devList[dev].fd );
    }
}

/***********************************************************/
static void getReadings(int portNumber, reading_t *sample) 
{
    int  dev, fd, port ;

    // Get Amps device for this port
    dev = portConfig[portNumber].AmpSpiDevNum ;
    port = portConfig[portNumber].ampsADC_chNum ;

    // Has it been opened?
    if( devList[dev].fd < 0 ) {
        // No, Open it
        (devList[dev].open)( dev );
    };
    fd = devList[dev].fd ;

    // get amps sample
    sample->Asamp = (devList[dev].xfer)( fd, port );

    // Get Volts device for this port
    dev = portConfig[portNumber].VoltSpiDevNum ;
    port = portConfig[portNumber].voltsADC_chNum ;

    // Has it been opened?
    if( devList[dev].fd < 0 ) {
        (devList[dev].open)( dev );
    };
    fd = devList[dev].fd ;

    // get volts sample (16bit fraction of 4.096 volts)
    sample->Vsamp = (devList[dev].xfer)( fd, port );

}   // end get_readings()


/***********************************************************/
static void calcValues(int portNumber, reading_t *sample)
{

    // calculate miliamps
    sample->miliamps = SAMPLE2MAMPS(sample->Asamp);

    // calculate volts
    switch (portMap[portNumber].portVoltage) {
        default:
        case VOLT_SCALE:  // Read Vcc/Vref
            sample->Vsamp *= 64 ; // Perform Vsamp scaling
            sample->milivolts = (4096.0*65536)/(sample->Vsamp) ;
            sample->miliamps  = (4096.0*1024)/(sample->Asamp) ;
            break ;

        case VOLT_3_3:
            sample->milivolts = SAMPLE_TO_3_3VOLTS(sample->Vsamp);
            break;

        case VOLT_5:
            sample->milivolts = SAMPLE_TO_5VOLTS(sample->Vsamp);
            break;

        case VOLT_12:
            sample->milivolts = SAMPLE_TO_12VOLTS(sample->Vsamp);
            break;

    }
 
    // calculate miliwatts
    sample->miliwatts = sample->miliamps * sample->milivolts;
    sample->miliwatts /= 1000;  // convert from E-6 to E-3

}   // end calcValues()


void pidev_read(int portNumber, reading_t *sample)
{
        // Collect raw readings
        getReadings(portNumber, sample);

        // Calculate power
        calcValues(portNumber, sample); 
}

void pidev_close(void)
{
    int i;

    for( i = sizeof(devList)/sizeof(*devList)-1 ; i > 0 ; --i ) {
        (devList[i].close)( i );
    }
}

