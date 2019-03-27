
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lm_sensor.h"
#include "pwr_dev.h"

#define GENERATE_STRING(STRING) #STRING,

const char* SensorNames[] = {
	FOREACH_ENUM(GENERATE_STRING)
};


const char* sensorName( SensorInfo* info )
{
	return info->name;
}

double sensorRead( SensorInfo* info )
{
	double value;
	FILE* fp = fopen(info->filename,"r");
	assert( 1 == fscanf( fp,"%lf", &value ) );
	fclose( fp );
	return value/info->scale;
}

SensorInfo* sensorInit( int index, int lm_id, const char* path, const char* type )
{
	SensorInfo* info = malloc( sizeof(SensorInfo) );
	info->name = SensorNames[index];
	sprintf( info->filename, "%s/%s%d_input",path,type,lm_id);	
	DBGP("%s() index=%d lm_id=%d %s %s\n",__func__,index,lm_id,info->name, info->filename);
	info->scale = 1000000.0;
	FILE* fp = fopen(info->filename,"r");
	if ( ! fp ) {
		free( info );
		info = NULL;
	} else {
		fclose(fp);
	}
	return info;
}

int sensorFind(const char* path, const char* type, const char* label)
{
	DBGP("%s() label=`%s`\n",__func__,label);
	char* buf=NULL;
	int cnt=1;
	while( 1 ) {
		char filename[1024];		
		sprintf( filename, "%s/%s%d_label",path,type,cnt);	
		FILE* fp = fopen(filename,"r");
		if ( NULL == fp ) {
			break;
		}
		size_t len=0;
		getline(&buf,&len,fp);
		fclose(fp);
		buf[strlen(buf)] = 0;
		int i;
		for ( i = strlen(buf) -1; i >= 0; i-- ) {
			if ( ! isspace( buf[i] ) ) {
				break;
			}
			buf[i] = 0;
		} 

		if ( 0 == strcmp( buf, label ) ) {
			free(buf);
			return cnt;
		} 
		free(buf);

		++cnt;
	}
	return -1;
}

