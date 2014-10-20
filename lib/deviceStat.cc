#include "deviceStat.h"

using namespace PowerAPI;

int DeviceStat::start( ) {
	DBGX("%s\n",objTypeToString(m_obj->type()));
	return m_obj->attrStartStat( m_attrName, m_attrStat );
}

int DeviceStat::stop( ) {
	DBGX("%s\n",objTypeToString(m_obj->type()));
	return PWR_RET_SUCCESS;
	return m_obj->attrStopStat( m_attrName, m_attrStat );
}

int DeviceStat::clear( ) {
	DBGX("%s\n",objTypeToString(m_obj->type()));
	return PWR_RET_SUCCESS;
	return m_obj->attrClearStat( m_attrName, m_attrStat );
}

int DeviceStat::getValue( double* value, PWR_StatTimes* statTimes ) {
	DBGX("%s\n",objTypeToString(m_obj->type()));
	return m_obj->attrGetStat( m_attrName, m_attrStat, value, statTimes );
}

int DeviceStat::getValues( double value[], PWR_StatTimes statTimes[] ) 
{
	DBGX("%s\n",objTypeToString(m_obj->type()));
	for ( int i = 0; i < m_grp->size(); i++ ) {
		TreeNode* obj = m_grp->getObj(i);	
		int retval = obj->attrGetStats( m_attrName, m_attrStat,
								&value[i], &statTimes[i] );
		if ( retval != PWR_RET_SUCCESS ) {
			return PWR_RET_FAILURE;
		} 	
	}
	return PWR_RET_SUCCESS;
}
