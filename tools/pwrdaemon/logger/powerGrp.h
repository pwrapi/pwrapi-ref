#ifndef _POWER_GRP_H
#define _POWER_GRP_H

#include <vector>
#include "work.h" 
#include "util.h"

namespace PWR_Logger {
class PowerGrp : public Work {
  public:
	PowerGrp( PWR_Cntxt ctx, std::string name ) : m_grpName(name) {
    	int rc = PWR_CntxtGetGrpByName( ctx, name.c_str(), &m_grp );
    	assert( rc == PWR_RET_SUCCESS );
	
	}


    int work( FILE* );

  private:
	PWR_Grp 	m_grp;
	std::string m_grpName;
};

int PowerGrp::work( FILE* fp )
{
	const char* attrName = attrNameToString( PWR_ATTR_POWER);

    fprintf(fp,"Logger: grpName=\'%s\' attr=%s\n",
							m_grpName.c_str(), attrName );

	
	int size = PWR_GrpGetNumObjs( m_grp );
    std::vector<double> value( size );
    std::vector<PWR_Time> ts(size);
	fprintf(fp,"%d\n",size);
        fflush(fp);

	PWR_Status status;	
	int rc = PWR_StatusCreate( &status );
	assert( rc = PWR_RET_SUCCESS );

    while( 1 ) {

        int retval = PWR_GrpAttrGetValue( m_grp, PWR_ATTR_POWER, 
				&value[0], &ts[0], status );

        if( retval != PWR_RET_SUCCESS ) {
            fprintf(fp,"ERROR: PWR_ObjAttrGetValue( `%s`, `%s` ) failed,"
						" retval=%d\n", m_grpName.c_str(), attrName, retval );
			fflush(fp);
			return -1;
        }

		for ( int i = 0; i < size; i++ ) {
        	fprintf(fp,"Logger: %.2f Watts, time %lf seconds\n", 
									value[i], (double)ts[i]/1000000000.0);
		}
        fflush(fp);
        sleep(1);
    }
	return 0;
}

}

#endif
