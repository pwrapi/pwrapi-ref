/*
 * p9_inband_sensors.c : C wrapper to read all the inband sensors in P9
 *
 * To build:
 * # gcc p9_inband_sensors.c -lm -o p9sensors
 *
 * To run:
 * # ./p9sensors  --> Default read Chip 0's data
 * # ./p9sensors <occ_num>
 * # ./p9sensors 0  --> Chip 0
 * # ./p9sensors 1  --> Chip 1
 */

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <math.h>
#include "occ_sensor.h"

/*
 * OCC Sensor Data
 *
 * OCC sensor data will use BAR2 (OCC Common is per physical drawer).
 * Starting address is at offset 0x00580000 from BAR2 base address.
 * Maximum size is 1.5MB.
 *
 * -------------------------------------------------------------------------
 * | Start (Offset from |    End    | Size       |Description           |
 * | BAR2 base address) |        |       |               |
 * -------------------------------------------------------------------------
 * |    0x00580000      |  0x005A57FF   |150kB     |OCC 0 Sensor Data Block|
 * |    0x005A5800      |  0x005CAFFF   |150kB       |OCC 1 Sensor Data Block|
 * |        :        |    :    |  :       |        :          |
 * |    0x00686800    |  0x006ABFFF   |150kB       |OCC 7 Sensor Data Block|
 * |    0x006AC000    |  0x006FFFFF   |336kB     |Reserved           |
 * -------------------------------------------------------------------------
 *
 *
 * OCC N Sensor Data Block Layout (150kB)
 *
 * The sensor data block layout is the same for each OCC N. It contains
 * sensor-header-block, sensor-names buffer, sensor-readings-ping buffer and
 * sensor-readings-pong buffer.
 *
 * ----------------------------------------------------------------------------
 * | Start (Offset from OCC |   End       | Size |Description              |
 * | N Sensor Data Block)   |           |      |                  |
 * ----------------------------------------------------------------------------
 * |    0x00000000        |  0x000003FF  |1kB   |Sensor Data Header Block   |
 * |    0x00000400        |  0x0000CBFF  |50kB  |Sensor Names              |
 * |    0x0000CC00        |  0x0000DBFF  |4kB   |Reserved              |
 * |    0x0000DC00        |  0x00017BFF  |40kB  |Sensor Readings ping buffer|
 * |    0x00017C00        |  0x00018BFF  |4kB   |Reserved              |
 * |    0x00018C00        |  0x00022BFF  |40kB  |Sensor Readings pong buffer|
 * |    0x00022C00        |  0x000257FF  |11kB  |Reserved              |
 * ----------------------------------------------------------------------------
 *
 * Sensor Data Header Block : This is written once by the OCC during
 * initialization after a load or reset. Layout is defined in 'struct
 * occ_sensor_data_header'
 *
 * Sensor Names : This is written once by the OCC during initialization after a
 * load or reset. It contains static information for each sensor. The number of
 * sensors, format version and length of each sensor is defined in
 * 'Sensor Data Header Block'. Format of each sensor name is defined in
 * 'struct occ_sensor_name'. The first sensor starts at offset 0 followed
 * immediately by the next sensor.
 *
 * Sensor Readings Ping/Pong Buffer:
 * There are two 40kB buffers to store the sensor readings. One buffer that
 * is currently being updated by the OCC and one that is available to be read.
 * Each of these buffers will be of the same format. The number of sensors and
 * the format version of the ping and pong buffers is defined in the
 * 'Sensor Data Header Block'.
 *
 * Each sensor within the ping and pong buffers may be of a different format
 * and length. For each sensor the length and format is determined by its
 * 'struct occ_sensor_name.structure_type' in the Sensor Names buffer.
 *
 * --------------------------------------------------------------------------
 * | Offset | Byte0 | Byte1 | Byte2 | Byte3 | Byte4 | Byte5 | Byte6 | Byte7 |
 * --------------------------------------------------------------------------
 * | 0x0000 |Valid  |           Reserved                    |
 * |        |(0x01) |                                |
 * --------------------------------------------------------------------------
 * | 0x0008 |            Sensor Readings                    |
 * --------------------------------------------------------------------------
 * |    :   |                :                    |
 * --------------------------------------------------------------------------
 * | 0xA000 |                     End of Data                    |
 * --------------------------------------------------------------------------
 *
 */

#define MAX_OCCS			8
#define MAX_CHARS_SENSOR_NAME		16
#define MAX_CHARS_SENSOR_UNIT		4

#define OCC_SENSOR_DATA_BLOCK_OFFSET	0x00580000
#define OCC_SENSOR_DATA_BLOCK_SIZE	0x00025800

enum occ_sensor_type {
	OCC_SENSOR_TYPE_GENERIC		= 0x0001,
	OCC_SENSOR_TYPE_CURRENT		= 0x0002,
	OCC_SENSOR_TYPE_VOLTAGE		= 0x0004,
	OCC_SENSOR_TYPE_TEMPERATURE	= 0x0008,
	OCC_SENSOR_TYPE_UTILIZATION	= 0x0010,
	OCC_SENSOR_TYPE_TIME		= 0x0020,
	OCC_SENSOR_TYPE_FREQUENCY	= 0x0040,
	OCC_SENSOR_TYPE_POWER		= 0x0080,
	OCC_SENSOR_TYPE_PERFORMANCE	= 0x0200,
};

enum occ_sensor_location {
	OCC_SENSOR_LOC_SYSTEM		= 0x0001,
	OCC_SENSOR_LOC_PROCESSOR	= 0x0002,
	OCC_SENSOR_LOC_PARTITION	= 0x0004,
	OCC_SENSOR_LOC_MEMORY		= 0x0008,
	OCC_SENSOR_LOC_VRM		= 0x0010,
	OCC_SENSOR_LOC_OCC		= 0x0020,
	OCC_SENSOR_LOC_CORE		= 0x0040,
	OCC_SENSOR_LOC_GPU		= 0x0080,
	OCC_SENSOR_LOC_QUAD		= 0x0100,
};

enum sensor_struct_type {
	OCC_SENSOR_READING_FULL		= 0x01,
	OCC_SENSOR_READING_COUNTER	= 0x02,
};

/**
 * struct occ_sensor_data_header -    Sensor Data Header Block
 * @valid:                When the value is 0x01 it indicates
 *                    that this header block and the sensor
 *                    names buffer are ready
 * @version:                Format version of this block
 * @nr_sensors:                Number of sensors in names, ping and
 *                    pong buffer
 * @reading_version:            Format version of the Ping/Pong buffer
 * @names_offset:            Offset to the location of names buffer
 * @names_version:            Format version of names buffer
 * @names_length:            Length of each sensor in names buffer
 * @reading_ping_offset:        Offset to the location of Ping buffer
 * @reading_pong_offset:        Offset to the location of Pong buffer
 * @pad/reserved:            Unused data
 */
struct occ_sensor_data_header {
	uint8_t valid;
	uint8_t version;
	uint16_t nr_sensors;
	uint8_t reading_version;
	uint8_t pad[3];
	uint32_t names_offset;
	uint8_t names_version;
	uint8_t name_length;
	uint16_t reserved;
	uint32_t reading_ping_offset;
	uint32_t reading_pong_offset;
} __attribute__((__packed__));

/**
 * struct occ_sensor_name -        Format of Sensor Name
 * @name:                Sensor name
 * @units:                Sensor units of measurement
 * @gsid:                Global sensor id (OCC)
 * @freq:                Update frequency
 * @scale_factor:            Scaling factor
 * @type:                Sensor type as defined in
 *                    'enum occ_sensor_type'
 * @location:                Sensor location as defined in
 *                    'enum occ_sensor_location'
 * @structure_type:            Indicates type of data structure used
 *                    for the sensor readings in the ping and
 *                    pong buffers for this sensor as defined
 *                    in 'enum sensor_struct_type'
 * @reading_offset:            Offset from the start of the ping/pong
 *                    reading buffers for this sensor
 * @sensor_data:            Sensor specific info
 * @pad:                Padding to fit the size of 48 bytes.
 */
struct occ_sensor_name {
	char name[MAX_CHARS_SENSOR_NAME];
	char units[MAX_CHARS_SENSOR_UNIT];
	uint16_t gsid;
	uint32_t freq;
	uint32_t scale_factor;
	uint16_t type;
	uint16_t location;
	uint8_t structure_type;
	uint32_t reading_offset;
	uint8_t sensor_data;
	uint8_t pad[8];
} __attribute__((__packed__));

/**
 * struct occ_sensor_record -        Sensor Reading Full
 * @gsid:                Global sensor id (OCC)
 * @timestamp:                Time base counter value while updating
 *                    the sensor
 * @sample:                Latest sample of this sensor
 * @sample_min:                Minimum value since last OCC reset
 * @sample_max:                Maximum value since last OCC reset
 * @csm_min:                Minimum value since last reset request
 *                    by CSM (CORAL)
 * @csm_max:                Maximum value since last reset request
 *                    by CSM (CORAL)
 * @profiler_min:            Minimum value since last reset request
 *                    by profiler (CORAL)
 * @profiler_max:            Maximum value since last reset request
 *                    by profiler (CORAL)
 * @job_scheduler_min:            Minimum value since last reset request
 *                    by job scheduler(CORAL)
 * @job_scheduler_max:            Maximum value since last reset request
 *                    by job scheduler (CORAL)
 * @accumulator:            Accumulator for this sensor
 * @update_tag:                Count of the number of ticks that have
 *                    passed between updates
 * @pad:                Padding to fit the size of 48 bytes
 */
struct occ_sensor_record {
	uint16_t gsid;
	uint64_t timestamp;
	uint16_t sample;
	uint16_t sample_min;
	uint16_t sample_max;
	uint16_t csm_min;
	uint16_t csm_max;
	uint16_t profiler_min;
	uint16_t profiler_max;
	uint16_t job_scheduler_min;
	uint16_t job_scheduler_max;
	uint64_t accumulator;
	uint32_t update_tag;
	uint8_t pad[8];
} __attribute__((__packed__));

/**
 * struct occ_sensor_counter -        Sensor Reading Counter
 * @gsid:                Global sensor id (OCC)
 * @timestamp:                Time base counter value while updating
 *                    the sensor
 * @accumulator:            Accumulator/Counter
 * @sample:                Latest sample of this sensor (0/1)
 * @pad:                Padding to fit the size of 24 bytes
 */
struct occ_sensor_counter {
	uint16_t gsid;
	uint64_t timestamp;
	uint64_t accumulator;
	uint8_t sample;
	uint8_t pad[5];
} __attribute__((__packed__));

static unsigned long read_counter(struct occ_sensor_data_header *hb, uint32_t offset)
{
	struct occ_sensor_counter *sping, *spong;
	struct occ_sensor_counter *sensor = NULL;
	uint8_t *ping, *pong;

	ping = (uint8_t *)((uint64_t)hb + be32toh(hb->reading_ping_offset));
	pong = (uint8_t *)((uint64_t)hb + be32toh(hb->reading_pong_offset));
	sping = (struct occ_sensor_counter *)((uint64_t)ping + offset);
	spong = (struct occ_sensor_counter *)((uint64_t)pong + offset);

	if (*ping && *pong) {
		if (be64toh(sping->timestamp) > be64toh(spong->timestamp))
			sensor = sping;
		else
			sensor = spong;
	} else if (*ping && !*pong) {
		sensor = sping;
	} else if (!*ping && *pong) {
		sensor = spong;
	} else if (!*ping && !*pong) {
		return 0;
	}

	return be64toh(sensor->accumulator);
}

enum sensor_attr {
	SENSOR_SAMPLE,
	SENSOR_ACCUMULATOR,
};

static unsigned long read_sensor(struct occ_sensor_data_header *hb, uint32_t offset,
			  int attr)
{
	struct occ_sensor_record *sping, *spong;
	struct occ_sensor_record *sensor = NULL;
	uint8_t *ping, *pong;

	ping = (uint8_t *)((uint64_t)hb + be32toh(hb->reading_ping_offset));
	pong = (uint8_t *)((uint64_t)hb + be32toh(hb->reading_pong_offset));
	sping = (struct occ_sensor_record *)((uint64_t)ping + offset);
	spong = (struct occ_sensor_record *)((uint64_t)pong + offset);

	if (*ping && *pong) {
		if (be64toh(sping->timestamp) > be64toh(spong->timestamp))
			sensor = sping;
		else
			sensor = spong;
	} else if (*ping && !*pong) {
		sensor = sping;
	} else if (!*ping && *pong) {
		sensor = spong;
	} else if (!*ping && !*pong) {
		return 0;
	}

	switch (attr) {
	case SENSOR_SAMPLE:
		return be16toh(sensor->sample);
	case SENSOR_ACCUMULATOR:
		return be64toh(sensor->accumulator);
	default:
		break;
	}

	return 0;
}

#define TO_FP(f)    ((f >> 8) * pow(10, ((int8_t)(f & 0xFF))))

typedef enum {
	Power,
	Energy,
} Type;

static uint64_t parseData( void *buf, int index, Type type)
{
	struct occ_sensor_data_header *hb;
	struct occ_sensor_name *md;
	int i = index;

	hb = (struct occ_sensor_data_header *)(uint64_t)buf;
	md = (struct occ_sensor_name *)((uint64_t)hb +
			be32toh(hb->names_offset));

	uint32_t offset =  be32toh(md[i].reading_offset);
	uint32_t scale = be32toh(md[i].scale_factor);
	uint64_t sample;

	if (md[i].structure_type == OCC_SENSOR_READING_FULL)
		sample = read_sensor(hb, offset, SENSOR_SAMPLE);
	else
		sample = read_counter(hb, offset);

	if (be16toh(md[i].type) ==OCC_SENSOR_TYPE_POWER) {
		uint64_t energy = read_sensor(hb, offset,
						      SENSOR_ACCUMULATOR);
		uint32_t freq = be32toh(md[i].freq);

		if ( type == Energy ) {
			return (uint64_t)(energy / TO_FP(freq));
		} else if ( type == Power ) {
			return (uint64_t)(sample * TO_FP(scale));
		} else {
			assert(0);
		}
	} else {
		return (uint64_t)(sample * TO_FP(scale));
	}
	assert(0);
}

static char lookup [NUM][2] = {{ 0,158 },{0,159},{1,158},{0,179},{1,162},{0,176},{1,159}};

static int chipId( OCC_Sensors sensor ) {
	return lookup[sensor][0];
}

static int sensorId( OCC_Sensors sensor ) {
	return lookup[sensor][1];
}

size_t OCC_dataSize( ) {
	return OCC_SENSOR_DATA_BLOCK_SIZE;
}

uint64_t OCC_readPower( char* buf , OCC_Sensors sensor ) {
	return parseData( buf, sensorId(sensor), Power );
}

uint64_t OCC_readEnergy( char* buf, OCC_Sensors sensor ) {
	return parseData( buf, sensorId(sensor), Energy );
}

int OCC_readData( int fd, OCC_Sensors sensor, char* buf, size_t length )
{
	int chipid = chipId(sensor);
	int rc, bytes;

	assert( length == OCC_SENSOR_DATA_BLOCK_SIZE );
	lseek(fd, chipid * OCC_SENSOR_DATA_BLOCK_SIZE, SEEK_SET);
	for (rc = bytes = 0; bytes < OCC_SENSOR_DATA_BLOCK_SIZE; bytes += rc) {
		rc = read(fd, buf + bytes, OCC_SENSOR_DATA_BLOCK_SIZE - bytes);
		if (!rc || rc < 0)
			break;
	}

	if (bytes != OCC_SENSOR_DATA_BLOCK_SIZE) {
		printf("Failed to read data\n");
		return -1;
	}
	return 0;
}
