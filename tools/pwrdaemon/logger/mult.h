
#ifndef _MULT_H
#define _MULT_H

#include "work.h" 

#include <util.h>
#include "./util.h"

namespace PWR_Logger {

class Mult : public Work {
  public:
	Mult( PWR_Cntxt ctx, std::string objName, std::string attrs ) : m_ctx(ctx) {

		if ( ! objName.compare(0,5,"TYPE:") ) {
			std::string type = objName.substr(5);
        	PWR_ObjType objType = objStringToType( type.c_str() );

        	int rc = PWR_CntxtGetGrpByType( ctx, objType, &m_grp );
        	assert( rc == PWR_RET_SUCCESS );
		} else {
			PWR_Obj obj;
    		PWR_CntxtGetObjByName( ctx, objName.c_str(), &obj );

    		assert( PWR_NULL != obj );
			int rc = PWR_GrpCreate( ctx, &m_grp );
        	assert( rc == PWR_RET_SUCCESS );
			rc = PWR_GrpAddObj( m_grp, obj ); 
        	assert( rc == PWR_RET_SUCCESS );
		}

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
	PWR_Cntxt   m_ctx;
	PWR_Grp 	m_grp;
	std::vector<PWR_AttrName> m_attrs;
};

int Mult::work( FILE* fp )
{
	int rc;

	size_t numObjs = PWR_GrpGetNumObjs( m_grp );

	PWR_Status status;
	PWR_StatusCreate( m_ctx, &status );
    while( 1 ) {
        std::vector<double> value(m_attrs.size() * numObjs );
        std::vector<PWR_Time> ts(m_attrs.size() * numObjs );
        rc = PWR_GrpAttrGetValues( m_grp, m_attrs.size(), &m_attrs[0], 
					&value[0], &ts[0], status );

        if( rc != PWR_RET_SUCCESS ) {
            fprintf(fp,"ERROR: PWR_GrpAttrGetValues( `) failed,"
						" retval=%d\n", rc );
			fflush(fp);
			return -1;
        }
		
		for ( unsigned j = 0; j < numObjs; j++ ) {
       		fprintf(fp,"Logger: `%s`:\n", getName( m_grp, j).c_str() );

			for ( unsigned i = 0; i < m_attrs.size(); i++ ) {
				int index = j * m_attrs.size() + i;
        		fprintf(fp,"Logger:   %10s %15.2f %-10s %lf seconds\n", 
					attrNameToString( m_attrs[i]), value[index], 
					attrUnitsToString( m_attrs[i]), 
							(double)ts[index]/1000000000.0);
			}
		}

		PWR_StatusClear( status );

        fflush(fp);
        sleep(1);
    }

	PWR_StatusDestroy( status );
	return 0;
}

}

#endif
