
#ifndef _LM_SENSOR_H
#define _LM_SENSOR_H

#include <limits.h>

#define FOREACH_ENUM(NAME) \
        NAME( System ) \
        NAME( GPU_0 ) \
        NAME( GPU_1 ) \
        NAME( CPU_0 ) \
        NAME( CPU_1 ) \

#define GENERATE_ENUM(ENUM) ENUM,

typedef enum {
        FOREACH_ENUM(GENERATE_ENUM)
        NumSensors
} SensorName;

typedef struct {
	double scale;
	const char* name;
	char filename[PATH_MAX];
} SensorInfo;

int sensorFind(const char* path, const char* type, const char* label);
SensorInfo* sensorInit( int i, int lm_id, const char* path, const char* type );
double sensorRead( SensorInfo* info );
const char* sensorName( SensorInfo* info );

#endif
