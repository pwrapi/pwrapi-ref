
#ifndef _ENERGY_H
#define _ENERGY_H

#include "work.h" 
#include "util.h"

namespace PWR_Logger {
class Energy : public Work {
  public:
    Energy( PWR_Cntxt ctx, std::string name ) {
        PWR_CntxtGetObjByName( ctx, name.c_str(), &m_obj );
        assert( PWR_NULL != m_obj );
    }
    int work( FILE* );
  private:
	PWR_Obj 	m_obj;
};

int Energy::work( FILE* fp )
{
	const char* attrName = attrNameToString( PWR_ATTR_POWER);
	char objName[100];
	PWR_ObjGetName( m_obj ,objName, 100 );

    fprintf(fp,"Logger: objName=\'%s\' attr=%s\n", objName, attrName );

	double startValue = 0;
    while( 1 ) {
        double value = 0;
        PWR_Time ts;
        int retval = PWR_ObjAttrGetValue( m_obj, PWR_ATTR_ENERGY, &value, &ts );

        if( retval != PWR_RET_SUCCESS ) {
            fprintf(fp,"ERROR: PWR_ObjAttrGetValue( `%s`, `%s` ) "
				"failed, retval=%d\n", objName, attrName, retval );
			fflush(fp);
			return -1;
        }

		if ( 0 == startValue ) {
			startValue = value;
		} else { 
			double delta = value - startValue;
        	fprintf(fp,"Logger: %.2f Joules, time %lf seconds\n",
										delta, (double)ts/1000000000.0);
        	fflush(fp);
			startValue = value;
		}
        sleep(1);
    }
	return 0;
}

}

#endif
