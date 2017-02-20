#ifndef _POWER_GRP_H
#define _POWER_GRP_H

#include <vector>
#include "work.h" 
#include "util.h"
#include "timeUtil.h"

namespace PWR_Logger {
class PowerGrp : public Work {
  public:

	PowerGrp( PWR_Cntxt ctx, std::string type ) : m_ctx(ctx), m_grpName(type) {
		
		PWR_ObjType objType = objStringToType( type.c_str() );	
		
    	int rc = PWR_CntxtGetGrpByType( ctx, objType, &m_grp );
    	assert( rc == PWR_RET_SUCCESS );
	}

    int work( FILE* );

  private:
	PWR_Cntxt   m_ctx;
	PWR_Grp 	m_grp;
	std::string m_grpName;
};

int PowerGrp::work( FILE* fp )
{
	const char* attrName = attrNameToString( PWR_ATTR_POWER);

    fprintf(fp,"Logger: grpName=\'%s\' attr=%s\n",
							m_grpName.c_str(), attrName );
    fflush(fp);
	
	int size = PWR_GrpGetNumObjs( m_grp );
    std::vector<double> value( size );
    std::vector<PWR_Time> ts(size);

	PWR_Status status;	
	int rc = PWR_StatusCreate( m_ctx, &status );
	assert( PWR_RET_SUCCESS == rc );

	int count = 0;
	if ( count ) {
		sleep(10);
	}
	double start = getTime();	
	for ( int i = -1; i < count; ) {

        int retval = PWR_GrpAttrGetValue( m_grp, PWR_ATTR_POWER, 
				&value[0], &ts[0], status );

        if( retval != PWR_RET_SUCCESS ) {
            fprintf(fp,"ERROR: PWR_ObjAttrGetValue( `%s`, `%s` ) failed,"
						" retval=%d\n", m_grpName.c_str(), attrName, retval );
			fflush(fp);
			return -1;
        }

		if ( count ) {
			++i;
			continue; 
		}
		
		for ( int i = 0; i < size; i++ ) {
			PWR_Obj obj;
			PWR_GrpGetObjByIndx( m_grp, i, &obj );
			char name[100];
			PWR_ObjGetName( obj, name, 100 );
        	fprintf(fp,"Logger: '%s', %.2f Watts, time %lf seconds\n", name, 
									value[i], (double)ts[i]/1000000000.0);
		}
        fflush(fp);
        sleep(1);
    }
	double stop = getTime();

    fprintf(fp,"samples=%d, total time=%f ms, latency=%f ms\n",
                                count,stop-start, (stop-start)/count);
    fflush( fp );
	return 0;
}

}

#endif
