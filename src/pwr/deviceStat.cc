/* 
 * Copyright 2014-2015 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#include <sys/time.h>
#include "deviceStat.h"

using namespace PowerAPI;

DeviceStat::~DeviceStat() {
	DBGX("\n");
	if ( m_isLogging ) {
		stop();
	}
}

static double getTime() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    double value;
    value = tv.tv_sec;
    value += (double) tv.tv_usec / 1000000000.0;
    return value;
}

int DeviceStat::start( ) {

	m_isLogging = true;
	m_startTime = getTime();
	m_period = 1.0/10.0;
	if ( m_obj ) {
		return startObj();
	} else {
		return startGrp();
	}
}

int DeviceStat::startObj( ) {
	int retval = m_obj->attrStartLog( m_attrName );
	DBGX("%s time=%.9f\n",objTypeToString(m_obj->type()),m_startTime);
	return retval;
}

int DeviceStat::startGrp( ) {

	for ( int i = 0; i < m_grp->size(); i++ ) {
		Object* obj = m_grp->getObj(i);
		DBGX("%s\n",objTypeToString(obj->type()));
		int retval = obj->attrStartLog( m_attrName );
		if ( retval != PWR_RET_SUCCESS ) {
			return retval;
		}			
	}
	return PWR_RET_SUCCESS;
}
int DeviceStat::stopObj( ) {
	DBGX("%s time=%.9f\n",objTypeToString(m_obj->type()),m_startTime);
	int retval = m_obj->attrStopLog( m_attrName );
	return retval;
}

int DeviceStat::stopGrp( ) {
	DBGX("\n");
	for ( int i = 0; i < m_grp->size(); i++ ) {
		Object* obj = m_grp->getObj(i);
		DBGX("%s\n",objTypeToString(obj->type()));
		int retval = obj->attrStopLog( m_attrName );
		if ( retval != PWR_RET_SUCCESS ) {
			return retval;
		}			
	}
	return PWR_RET_SUCCESS;
}

int DeviceStat::stop( ) {
	m_isLogging = false;
	m_stopTime = getTime();
	DBGX("time=%.9f\n",m_stopTime);
	if ( m_obj ) {
		return stopObj();
	} else {
		return stopGrp();
	}
}

int DeviceStat::clear( ) {
	DBGX("%s\n",objTypeToString(m_obj->type()));
	m_startTime = getTime();
	return PWR_RET_SUCCESS;
}

int DeviceStat::getValue( double* value, PWR_StatTimes* statTimes ) {

	return objGetValue( m_obj, value, statTimes );
}

int DeviceStat::objGetValue( Object* obj, double* value,
								PWR_StatTimes* statTimes )
{
	double now = getTime();
	double windowTime = now - m_startTime;	
	unsigned int nSamples = windowTime / m_period;  
	DBGX("%s curTime=%.9f wndwLngth=%.9f smpls=%d\n",
				objTypeToString(obj->type()), now, windowTime, nSamples);

	std::vector<double> values(nSamples,0.0);
	PWR_Time timeStamp;
	int retval = obj->attrGetSamples( m_attrName, &timeStamp, 
								m_period, &nSamples, &values[0] );
	statTimes->start = m_startTime;
	statTimes->stop = now;
	statTimes->instant = PWR_TIME_NOT_SET;
	*value = opPtr( values );
	return retval;
}

int DeviceStat::getValues( double value[], PWR_StatTimes statTimes[] ) 
{
	DBGX("\n");
	for ( int i = 0; i < m_grp->size(); i++ ) {
		int retval  = objGetValue( m_grp->getObj(i), &value[i], &statTimes[i] );
		if ( retval != PWR_RET_SUCCESS ) {
			return retval;
		} 	
	}
	return PWR_RET_SUCCESS;
}

