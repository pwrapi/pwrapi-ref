
#ifndef _POWER_H
#define _POWER_H

#include "work.h" 
#include <util.h>

namespace PWR_Logger {
class Power : public Work {
  public:
	Power( PWR_Cntxt ctx, std::string name ) {
    	PWR_CntxtGetObjByName( ctx, name.c_str(), &m_obj );
    	assert( PWR_NULL != m_obj );
	}
    int work( FILE* );

  private:
	PWR_Obj 	m_obj;
};

int Power::work( FILE* fp )
{
	const char* attrName = attrNameToString( PWR_ATTR_POWER);
	char objName[100];
	PWR_ObjGetName( m_obj, objName, 100 );

    fprintf(fp,"Logger: objName=\'%s\' attr=%s\n", objName, attrName );

    while( 1 ) {
        double value = 0;
        PWR_Time ts;
        int retval = PWR_ObjAttrGetValue( m_obj, PWR_ATTR_POWER, &value, &ts );

        if( retval != PWR_RET_SUCCESS ) {
            fprintf(fp,"ERROR: PWR_ObjAttrGetValue( `%s`, `%s` ) failed,"
						" retval=%d\n", objName, attrName, retval );
			fflush(fp);
			return -1;
        }

        fprintf(fp,"Logger: %.2f Watts, time %lf seconds\n", 
									value, (double)ts/1000000000.0);
        fflush(fp);
        sleep(1);
    }
	return 0;
}

}

#endif
