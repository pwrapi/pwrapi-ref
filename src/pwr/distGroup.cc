
#include "distCntxt.h"
#include "distGroup.h"
#include "distObject.h"
#include "distGrpComm.h"
#include "distRequest.h"
#include "attrInfo.h"

#include <stdio.h>


using namespace PowerAPI;

Object* DistGrp::getObj( unsigned i ) {

    return static_cast<Object*>( m_allObjs[ i ] );
}

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
	m_allObjs.push_back( obj );
    return PWR_RET_SUCCESS;
}

int DistGrp::attrSetValue( PWR_AttrName type, void* ptr, Status* status )
{
    DBGX("\n");
	return attrSetValues( 1, &type, ptr, status );
}

int DistGrp::attrGetValue( PWR_AttrName type, void* ptr, PWR_Time ts[],
                            			Status* status ) 
{
    DBGX("\n");
	return attrGetValues( 1, &type, ptr, ts, status );
}

int DistGrp::attrSetValues( int num, PWR_AttrName attr[], void* buf,
                            			Status* status )
{
    DBGX("\n");

	for ( unsigned i = 0; i < m_list.size(); i++ ) {

		int rc = m_list[i]->attrSetValues( num, attr, 
					buf, status ); 
		if ( rc != PWR_RET_SUCCESS && rc != PWR_RET_STATUS ) {
			return rc;
		}
	}
	
	if ( ! m_distObjs.empty() ) {
	
		DistRequest distReq( m_ctx, status );
		if ( ! m_comm ) {
			m_comm = new DistGrpComm( 
					static_cast<DistCntxt*>(m_ctx), m_distObjs );
		}

        DistCommReq* commReq = new DistSetCommReq(&distReq);
        distReq.insert( commReq );

		m_comm->setValues( num, attr, buf, commReq ); 

		distReq.wait( );
	}
	
	return status->empty() ? PWR_RET_SUCCESS : PWR_RET_STATUS;
}

int DistGrp::attrGetValues( int num, PWR_AttrName attr[], void* buf,
                                        PWR_Time ts[], Status* status)
{
    DBGX("\n");
	uint64_t* ptr = (uint64_t*) buf;

	for ( unsigned i = 0; i < m_list.size(); i++ ) {

		int rc = m_list[i]->attrGetValues( num, attr, 
					ptr + i * num, ts + i * num, status ); 
		if ( rc != PWR_RET_SUCCESS && rc != PWR_RET_STATUS ) {
			return rc;
		}
	}
	
	if ( ! m_distObjs.empty() ) {
	
		std::vector<ValueOp> valueOp(num);

		// attributes are the same for all members of the group
		// or are they?
		for ( int i = 0; i < num; i++ ) { 
    		valueOp[i] = m_distObjs[0]->getAttrInfo( attr[i] ).valueOp;
		}

		DistRequest distReq( m_ctx, status );
		if ( ! m_comm ) {
			m_comm = new DistGrpComm( 
					static_cast<DistCntxt*>(m_ctx), m_distObjs );
		}

		distReq.value.resize( m_distObjs.size() );
		distReq.timeStamp.resize( m_distObjs.size() );

		for ( unsigned i = 0; i < m_distObjs.size(); i++ ) {
			for ( unsigned j = 0; j < m_allObjs.size(); j++ ) {
				if ( m_distObjs[i] == m_allObjs[j] ) {
					distReq.value[i] = ptr + i * valueOp.size(); 
					distReq.timeStamp[i] = ts + i * valueOp.size();
					break;
				}
			}			
		}

        DistCommReq* commReq = new DistGetCommReq(&distReq);
        distReq.insert( commReq );

		m_comm->getValues( num, attr, &valueOp[0], commReq ); 

		distReq.wait( );
		delete commReq;
	}
	
	return status->empty() ? PWR_RET_SUCCESS : PWR_RET_STATUS;
}
