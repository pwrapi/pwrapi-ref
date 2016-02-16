
#include "distGroup.h"
#include "distObject.h"

#include <stdio.h>

using namespace PowerAPI;

int DistGrp::add( Object* _obj )
{
	DistObject* obj = static_cast<DistObject*>(_obj);

	if ( obj->isLocal() ) {
		Grp::add( _obj );
	} else {

		DBGX("add remote %s\n",obj->name().c_str());	
		for ( unsigned i = 0; i < m_distObjs.size(); i ++ ) {
            if ( m_distObjs[i] == obj ) {
                DBGX("duplicate\n");
                return PWR_RET_FAILURE;
            }
        }
        m_distObjs.push_back( obj );
	}
    return PWR_RET_SUCCESS;
}

int DistGrp::attrSetValue( PWR_AttrName type, void* ptr, Status* status )
{
	return attrSetValues( 1, &type, ptr, status );
}

int DistGrp::attrGetValue( PWR_AttrName type, void* ptr, PWR_Time ts[],
                            			Status* status ) 
{
	return attrGetValues( 1, &type, ptr, ts, status );
}

int DistGrp::attrSetValues( int num, PWR_AttrName attr[], void* buf,
                            			Status* status )
{
	assert(0);
}

int DistGrp::attrGetValues( int num, PWR_AttrName attr[], void* buf,
                                        PWR_Time ts[], Status* status)
{
	uint64_t* ptr = (uint64_t*) buf;
	for ( unsigned i = 0; i < m_list.size(); i++ ) {

		int rc = m_list[i]->attrGetValues( num, attr, 
					ptr + i * num, ts + i * num, status ); 
		if ( rc != PWR_RET_SUCCESS && rc != PWR_RET_STATUS ) {
			return rc;
		}
	}
#if 0
		for ( int j = 0; j < num; j++ ) {
			DistObject* obj = static_cast<DistObject*>(m_list[i]); 
			printf("%d %d\n",i,j);
		}
#endif

	if ( ! m_distObjs.empty() ) {
		assert(0);
	}
	return PWR_RET_SUCCESS;
}
