#include "DeviceStat.h"

using namespace PowerAPI;

int DeviceStat::start( ) {
    return PWR_RET_SUCCESS;
}

int DeviceStat::stop( ) {
    return PWR_RET_SUCCESS;
}

int DeviceStat::clear( ) {
    return PWR_RET_FAILURE;
}

int DeviceStat::getValue( double* value, PWR_StatTimes* statTimes ) {
	DBGX("%s\n",objTypeToString(m_obj->type()));
	return m_obj->getStat( m_attrName, m_attrStat, value, statTimes );
}

int DeviceStat::getValues( double value[], PWR_StatTimes statTimes[] ) {
	DBGX("%s\n",objTypeToString(m_obj->type()));
	return m_obj->getStats( m_attrName, m_attrStat, value, statTimes );
}
