
#ifndef _RATE_H
#define _RATE_H

#include "work.h" 
#include "util.h"

namespace PWR_Logger {
class Rate : public Work {
  public:
    int work( PWR_Cntxt, PWR_Obj, FILE* );
};

#include <sys/time.h>

static inline double getTime() {
    struct timeval now;
    int rv = gettimeofday(&now, NULL);

    return (now.tv_sec * 1000) + ((double) now.tv_usec / 1000.0);
}

int Rate::work( PWR_Cntxt ctx, PWR_Obj obj, FILE* fp )
{
	const char* attrName = attrNameToString( PWR_ATTR_POWER);
	const char* objName;
	PWR_ObjGetName( obj ,&objName );

    fprintf(fp,"Logger: objName=\'%s\' attr=%s\n", objName, attrName );

	int count = 1000;
	double start = getTime();
	for ( int i = 0; i < count; i++ ) {
        double value = 0;
        PWR_Time ts;
        int retval = PWR_ObjAttrGetValue( obj, PWR_ATTR_ENERGY, &value, &ts );

        if( retval != PWR_RET_SUCCESS ) {
            fprintf(fp,"ERROR: PWR_ObjAttrGetValue( `%s`, `%s` ) failed, retval=%d\n",
					objName, attrName, retval );
			fflush(fp);
			return -1;
        }
	}
	double stop = getTime();
	fprintf(fp,"samples=%d, total time=%f ms, latency=%f ms\n",
								count,stop-start, (stop-start)/count);
	fflush( fp );

	return 0;
}

}

#endif
