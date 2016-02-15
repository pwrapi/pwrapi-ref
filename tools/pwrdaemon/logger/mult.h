
#ifndef _MULT_H
#define _MULT_H

#include "work.h" 
#include "util.h"

namespace PWR_Logger {

static inline PWR_AttrName getAttr( std::string name );

class Mult : public Work {
  public:
	Mult( PWR_Cntxt ctx, std::string objName, std::string attrs ) {

    	PWR_CntxtGetObjByName( ctx, objName.c_str(), &m_obj );
    	assert( PWR_NULL != m_obj );

		//printf("%s \n", attrs.c_str());
		size_t pos = 0;
		while ( pos < attrs.length() ) {
			size_t tmp = attrs.find_first_of( ',', pos );

			m_attrs.push_back( getAttr( attrs.substr(pos,tmp) ) );

			if ( tmp < std::string::npos ) {
				pos = tmp + 1;
			} else {
				pos = attrs.length();
			}
		}
	}
    int work( FILE* );

  private:
	PWR_Obj 	m_obj;
	std::vector<PWR_AttrName> m_attrs;
};

int Mult::work( FILE* fp )
{
	char objName[100];
	PWR_ObjGetName( m_obj, objName, 100 );

    fprintf(fp,"Logger: objName=\'%s\'\n", objName );

    while( 1 ) {
        std::vector<double> value(m_attrs.size());
        std::vector<PWR_Time> ts(m_attrs.size());
		PWR_Status status;
		PWR_StatusCreate( &status );
        int retval = PWR_ObjAttrGetValues( m_obj, m_attrs.size(), &m_attrs[0], 
					&value[0], &ts[0], status );

        if( retval != PWR_RET_SUCCESS ) {
            fprintf(fp,"ERROR: PWR_ObjAttrGetValues( `%s`, `%s` ) failed,"
						" retval=%d\n", objName, "??", retval );
			fflush(fp);
			return -1;
        }
		
		for ( unsigned i = 0; i < m_attrs.size(); i++ ) {
        	fprintf(fp,"Logger: %s %.2f %s, time %lf seconds\n", 
				attrNameToString( m_attrs[i]), value[i], 
				attrUnitsToString( m_attrs[i]), (double)ts[i]/1000000000.0);
		}
        fflush(fp);
        sleep(1);
    }
	return 0;
}

PWR_AttrName getAttr( std::string name )
{
	printf("%s\n",name.c_str());
	if ( ! name.compare( "energy" ) ) {
		return PWR_ATTR_ENERGY;
	} else if ( ! name.compare( "power" ) ) {
		return PWR_ATTR_POWER;
	}
	assert(0);
}

}

#endif
