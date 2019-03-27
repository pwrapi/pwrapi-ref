
#ifndef _OCC_SENSOR_H
#define _OCC_SENSOR_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
        System,
        GPU_0,
        GPU_1,
        Memory_0,
        Memory_1,
        Socket_0,
        Socket_1,
        NUM
} OCC_Sensors;

uint64_t OCC_readPower( char* buf , OCC_Sensors sensor );
uint64_t OCC_readEnergy( char* buf, OCC_Sensors sensor );
size_t OCC_dataSize();
int OCC_readData( int fd, OCC_Sensors sensor, char* buf, size_t length );

#endif
