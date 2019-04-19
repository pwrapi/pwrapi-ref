

#include <ctype.h>
#include <string.h>
#include <pwr.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h>
#include <sys/time.h>
#include <pthread.h>
#include <inttypes.h>
#include "util.h"
#include <sys/stat.h>
#include <fcntl.h>

#define ONE_RANK_PER_NODE 0 

static void _pwr_runtime_init() __attribute__((constructor));
static void _pwr_runtime_fini() __attribute__((destructor));

typedef struct {
	PWR_Obj* objs;
	PWR_AttrName* attrs;
	char** names;
	char** units;
	int numObjs;
} SampleInfo; 

static struct Info {
	int fd;
	int jobid;
	int nodeid;
	FILE* logFile;
	char* energyObjName;
	double startTime;
	int localRank;
	double nodeEnergy;
	PWR_Obj   node;
	int threadRun;
	PWR_Cntxt cntxt;
	int verbose;
	pthread_t thread;
	SampleInfo* sampleInfo;
	useconds_t sleep_us;
} _rtInfo = { verbose : 0, threadRun: 0, sampleInfo : NULL, energyObjName : "plat.node0", localRank : -1 };


static inline double getTime_us() {
        struct timeval tv;
        gettimeofday(&tv,NULL);
        double value;
        value = tv.tv_sec * 1000000;
        value += tv.tv_usec;
        return value;
}

static SampleInfo* initSampleObjs( char* str );
static void* thread( void* arg );
static char* findFirst( char** str, char c ); 
static PWR_AttrName getAttr( const char * );
static char* toUpper( char* str );
static char* getUnits( PWR_AttrName );
static char* getLSBnodeid();

void _pwr_runtime_init(){

	char* tmp = getenv( "PWR_RUNTIME_VERBOSE" );

	if ( tmp && atoi(tmp) > 0 ) {
		_rtInfo.verbose = atoi(tmp);
	}

#if ONE_RANK_PER_NODE 
	tmp = getenv("PWR_RUNTIME_ID");

	if ( tmp ) {
		char filename[PATH_MAX];
		sprintf( filename, "/tmp/pwr_runtime-%d", atoi(tmp) );
		_rtInfo.localRank = creat( filename, O_CREAT );
		if ( _rtInfo.localRank >=0 ) { 
			_rtInfo.localRank = 0;
			if ( _rtInfo.verbose ) {
				printf("I won id = %d %d\n",atoi(tmp), _rtInfo.localRank );
			}
		}
	} else {
		tmp = getenv("OMPI_COMM_WORLD_NODE_RANK");
	
		if ( ! tmp ) {
			tmp = getenv("SLURM_LOCALID");
		}
		if ( ! tmp ) {
			fprintf(stderr,"could not determine node rank\n");
			exit(-1);
		}
		_rtInfo.localRank = atoi( tmp );
	}

	if ( 0 != _rtInfo.localRank ) {
		return;
	}
#endif

	tmp = getenv("LSB_JOBID");

	if ( ! tmp ) {
		tmp = getenv("SLURM_JOB_ID");
	}
	if ( ! tmp ) {
		fprintf(stderr,"could not determine job id\n");
		exit(-1);
	}

	_rtInfo.jobid = atoi(tmp);


	tmp = getenv("SLURM_NODEID");

	if ( ! tmp ) {
		tmp = getLSBnodeid();
	}

	if ( ! tmp ) {
		fprintf(stderr,"could not determine node id\n");
		exit(-1);
	}

	_rtInfo.nodeid = atoi( tmp );

	if ( _rtInfo.verbose ) {
		printf("%s() jobid=%d nodeid=%d pid=%d\n",__func__,_rtInfo.jobid, _rtInfo.nodeid, getpid());
	}

	char* dataDir = getenv( "PWR_RUNTIME_DIR");

	if ( ! dataDir ) {
		struct passwd *pw = getpwuid( getuid() );
		dataDir = pw->pw_dir;
	}

	char filename[PATH_MAX];
	sprintf( filename, "%s/pwr_runtime-%d", dataDir, _rtInfo.jobid );

#if ONE_RANK_PER_NODE 
	if ( 0 == _rtInfo.nodeid ) {
		struct stat statbuf;
		if ( 0 != stat( filename, &statbuf ) ) {
			int ret = mkdir( filename, 0700 );	
			if ( -1 == ret ) {
				fprintf(stderr,"could not make log directory %s\n",filename);
				exit(-1);
			}
		}
	} else {
#else 
	if (  -1 ==  mkdir( filename, 0700 ) ) {
#endif
		struct stat statbuf;
		
		int cnt = 60*5; 
		while ( cnt-- && 0 != stat( filename, &statbuf )  ) {
			sleep(1);
		}
		if ( stat( filename, &statbuf )  ) {
			fprintf(stderr, "node %d couldn't stat %s\n",_rtInfo.nodeid, filename );
			exit(-1);
		}
	}
	char hostname[100];	
	if ( 0 > gethostname(hostname,100) ) {
		fprintf(stderr, "failed to gethostname\n");
		return;	
	}
	sprintf( filename, "%s/pwr_runtime-%d/pid-%d_%s", dataDir, _rtInfo.jobid, getpid(), hostname );
	_rtInfo.logFile = fopen( filename, "w+");
	if ( ! _rtInfo.logFile ) { 
		fprintf(stderr, "node %d failed to open %s\n",_rtInfo.nodeid, filename );
		exit(-1);
	}
	
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char s[64];
	strftime(s, sizeof(s), "%c", tm);
	fprintf(_rtInfo.logFile,"# %s\n",s);

	fprintf(_rtInfo.logFile,"# JOBID: %d\n", _rtInfo.jobid );
	fprintf(_rtInfo.logFile,"# host: %s\n", hostname );

	_rtInfo.startTime = getTime_us();
	fprintf(_rtInfo.logFile,"# Start Time: %.6lf seconds\n",_rtInfo.startTime /1000000.0);

	int rc;
	rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "App", &_rtInfo.cntxt );
	if ( PWR_RET_SUCCESS != rc ) {
		fprintf(stderr,"PWR_CntxtInit() failed");
		exit(-1);
	}

	tmp = getenv( "PWR_RUNTIME_TOTAL_ENERGY_OBJ");
	if ( tmp ) {
		_rtInfo.energyObjName= tmp;	
	}
	rc = PWR_CntxtGetObjByName( _rtInfo.cntxt, _rtInfo.energyObjName, &_rtInfo.node );
	if ( PWR_RET_SUCCESS != rc ) {
		fprintf(stderr,"PWR_CntxtGetObjByName() failed");
		exit(-1);
	}

	PWR_Time time;
	rc = PWR_ObjAttrGetValue( _rtInfo.node, PWR_ATTR_ENERGY, (void*) &_rtInfo.nodeEnergy, &time );
	if ( PWR_RET_SUCCESS != rc ) {
		fprintf(stderr,"PWR_ObjAttrGetValue() failed");
		exit(-1);
	}
	
	char* hz = getenv("PWR_RUNTIME_SAMPLE_HZ");
	char* sampleObjs = getenv("PWR_RUNTIME_SAMPLE_OBJECTS");

	if ( hz || sampleObjs) {
		fprintf(_rtInfo.logFile,"# column 0: Time in seconds\n");
		_rtInfo.sampleInfo = initSampleObjs(sampleObjs);
		int i;
		for ( i = 0; i < _rtInfo.sampleInfo->numObjs; i++ ) {
			fprintf(_rtInfo.logFile,"# column %d: %s %s\n", i+1, _rtInfo.sampleInfo->names[i],
							_rtInfo.sampleInfo->units[i]);
		}
		_rtInfo.threadRun = 1;
		if ( ! hz )  {
			hz = "10";
		}
		_rtInfo.sleep_us = (1.0/atof(hz) * 1000000.0);
		pthread_create( &_rtInfo.thread, NULL, thread, _rtInfo.sampleInfo );
	}
}

void _pwr_runtime_fini(){


#if ONE_RANK_PER_NODE 
	if ( 0 != _rtInfo.localRank ) {
		return;
	}
#endif
#if 0
	char* tmp = getenv("PWR_RUNTIME_ID");

	if ( tmp ) {
		char filename[PATH_MAX];
		sprintf( filename, "/tmp/pwr_runtime-%d", atoi(tmp) );
		
		unlink( filename );
	}
#endif

	if ( _rtInfo.threadRun ) {
		_rtInfo.threadRun = 0;
		pthread_join( _rtInfo.thread, NULL );
	}

	if ( _rtInfo.verbose ) {
		printf("%s() pid=%d jobid=%d\n",__func__,_rtInfo.jobid,getpid());
	}

	PWR_Time time;
	double energy;
	int rc;
	rc = PWR_ObjAttrGetValue( _rtInfo.node, PWR_ATTR_ENERGY, (void*) &energy, &time );
	assert( PWR_RET_SUCCESS == rc );

	double totalTime = (getTime_us() - _rtInfo.startTime)/1000000.0;
	double totalEnergy = energy-_rtInfo.nodeEnergy;
	fprintf(_rtInfo.logFile,"# Total Time: %.6lf secconds\n",totalTime );
	fprintf(_rtInfo.logFile,"# Energy `%s`: %.0lf joules\n", _rtInfo.energyObjName, totalEnergy );
	fprintf(_rtInfo.logFile,"# Average Power `%s`: %.0lf watts\n", _rtInfo.energyObjName, totalEnergy/totalTime);

	fclose(_rtInfo.logFile );

        PWR_CntxtDestroy( _rtInfo.cntxt );	
}

static void* thread( void* arg ) {
	SampleInfo* info = arg;
	while( _rtInfo.threadRun ) {
		usleep( _rtInfo.sleep_us );
		PWR_Time time;
		double value;
		int i;
		fprintf(_rtInfo.logFile,"%lf", (getTime_us() - _rtInfo.startTime) / 1000000.0 );
		for ( i = 0; i < info->numObjs; i++ ) {
	        	int rc = PWR_ObjAttrGetValue( info->objs[i], info->attrs[i], &value, &time );

    			if( PWR_RET_SUCCESS != rc ) {
                		fprintf(_rtInfo.logFile,"Get Failed: %s: %s\n",info->names[i], attrNameToString(info->attrs[i]) );
        		} else {	
        			fprintf(_rtInfo.logFile," %.0lf", value);
			}
		}
		fprintf(_rtInfo.logFile,"\n");
	}
}

static SampleInfo* initSampleObjs( char* str ) {
	size_t len = strlen(str);		

	SampleInfo* info = malloc( sizeof( SampleInfo ) );
	info->numObjs = 0;
	size_t pos;
	for ( pos = 0; pos < len; pos++ ) {
		if ( str[pos] == ':' ) {
			++info->numObjs;
		}
	}
	if ( _rtInfo.verbose ) {
		fprintf(stdout,"found possible %d objects to sample\n",info->numObjs);
	}

	if ( 0 == info->numObjs ) {
		free(info);
		return NULL;
	}
	info->objs = malloc( sizeof (PWR_Obj) * info->numObjs );
	info->attrs = malloc( sizeof(PWR_AttrName) * info->numObjs );
	info->names = malloc( sizeof(char*) * info->numObjs );
	info->units = malloc( sizeof(char*) * info->numObjs );
	
	pos = 0;
	size_t delim;
	int objNum = 0;
	while( strlen(str) ) {
		info->names[objNum] = str;
		char* delim = findFirst( &str, ':' );
		if ( 0 ==  strlen(str ) ) {
			fprintf(stderr,"Invalid PWR_RUNTIME_SAMPLE_OBJECTS string\n");
			break;
		}
		char* attr = str;

		delim = findFirst( &str, ',');
		info->attrs[objNum] = getAttr( toUpper(attr) );
		if ( PWR_ATTR_INVALID == info->attrs[objNum] ) {
			fprintf(stderr,"Invalid attribute %s for object %s\n",attr,info->names[objNum]);
			break;
		}
		if ( _rtInfo.verbose ) {
			printf("found name=%s attr=%s\n",info->names[objNum],attr);
		}
		info->units[objNum] = getUnits( info->attrs[objNum] );
		++objNum;
	}
	if ( _rtInfo.verbose ) {
		printf("found %d objects\n",objNum);
	}
	info->numObjs = objNum;

	int i;
	for ( i = 0; i < info->numObjs; i++ ) {
		int rc = PWR_CntxtGetObjByName( _rtInfo.cntxt, info->names[i], &info->objs[i] );
		assert( PWR_RET_SUCCESS == rc );
	}
	return info;
}


static char* findFirst( char** str, char c ) {
	size_t pos = 0;
	while ( (*str)[pos] && (*str)[pos] != c ) {
		++pos;
	}
	char* delim =  &(*str)[pos];
	if ( *delim ) {
		*delim = 0;
		*str = delim + 1;
	} else {
		*str = delim;
	}
	return delim;
} 

static char* getLSBnodeid()
{
	char* tmp = getenv( "LSB_MCPU_HOSTS" );
	int i;
	char hostname[100];	
	if ( 0 > gethostname(hostname,100) ) {
		fprintf(stderr, "failed to gethostname\n");
		exit(-1);	
	}
	for ( i = 0; tmp[i] && tmp[i] != ' '; i++ ) { }  
	if ( 0 == strncmp( hostname, tmp, i ) ) {
		return "0";
	} else {
		return "-1";
	}
}

static char* toUpper( char* str )
{
        int i;
        for ( i = 0; i < strlen(str); i++ ) {
                str[i] = toupper(str[i]);
        }
	return str;
}

static char* getUnits( PWR_AttrName name )
{
	switch( name ) {
	case PWR_ATTR_ENERGY:
		return "joules";
	case PWR_ATTR_POWER:
		return "watts";
	} 
	return "?";
}

PWR_AttrName getAttr( const char * name )
{

        if ( ! strcmp( name, "PSTATE" ) ) {
                return PWR_ATTR_PSTATE;
        } else if ( ! strcmp( name, "CSTATE" ) ) {
                return PWR_ATTR_CSTATE;
        } else if ( ! strcmp( name, "CSTATE" ) ) {
                return PWR_ATTR_CSTATE;
        } else if ( ! strcmp( name, "SSTATE" ) ) {
                return PWR_ATTR_SSTATE;
        } else if ( ! strcmp( name, "CURRENT" ) ) {
                return PWR_ATTR_CURRENT;
        } else if ( ! strcmp( name, "VOLTAGE" ) ) {
                return PWR_ATTR_VOLTAGE;
        } else if ( ! strcmp( name, "POWER" ) ) {
                return PWR_ATTR_POWER;
        } else if ( ! strcmp( name, "POWER_LIMIT_MAX" ) ) {
                return PWR_ATTR_POWER_LIMIT_MAX;
        } else if ( ! strcmp( name, "FREQ" ) ) {
                return PWR_ATTR_FREQ;
        } else if ( ! strcmp( name, "ENERGY" ) ) {
                return PWR_ATTR_ENERGY;
        } else if ( ! strcmp( name, "TEMP" ) ) {
                return PWR_ATTR_TEMP;
        } else if ( ! strcmp( name, "OS_ID" ) ) {
                return PWR_ATTR_OS_ID;
        }
        return PWR_ATTR_INVALID;
}
