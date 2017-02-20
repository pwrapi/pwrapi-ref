
#ifndef _SET_MULT_H
#define _SET_MULT_H

#include "work.h" 
#include <util.h>
#include "./util.h"

namespace PWR_Logger {

class SetMult : public Work {
  public:
	SetMult( PWR_Cntxt ctx, std::string objName, std::string attrs ): m_ctx(ctx) {

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
		values.resize( m_attrs.size() );

		for ( unsigned i = 0; i < m_attrs.size(); i++ ) {
			values[i] = 1000.0 + i; 
		}
	}
    int work( FILE* );

  private:
	PWR_Cntxt   m_ctx;
	PWR_Grp 	m_grp;
	std::vector<PWR_AttrName> m_attrs;
    std::vector<double> values;
};

int SetMult::work( FILE* fp )
{
	int rc;

	PWR_Status status;
	rc = PWR_StatusCreate( m_ctx, &status );
	assert( rc == PWR_RET_SUCCESS );
	fprintf(fp,"call set\n");
	fflush(fp);
    rc = PWR_GrpAttrSetValues( m_grp, m_attrs.size(), &m_attrs[0], 
					&values[0], status );
	fprintf(fp,"set returned\n");
	fflush(fp);

    if( rc != PWR_RET_SUCCESS ) {
            fprintf(fp,"ERROR: PWR_GrpAttrGetValues( `) failed,"
						" retval=%d\n", rc );
			fflush(fp);
		return -1;
    }
	return 0;
}

}

#endif
