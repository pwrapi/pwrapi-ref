/* 
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <sys/time.h>
#include "deviceStat.h"

using namespace PowerAPI;

DeviceStat::~DeviceStat() {
	DBGX("\n");
	if ( m_isLogging ) {
		stop();
	}
}

static PWR_Time getTime() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    PWR_Time value;
    value = tv.tv_sec * 1000000000;
    value += tv.tv_usec * 1000;
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
	DBGX("%s time=%"PRIu64" sec\n",objTypeToString(m_obj->type()),m_startTime);
	return retval;
}

int DeviceStat::startGrp( ) {

	for ( unsigned i = 0; i < m_grp->size(); i++ ) {
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
	DBGX("%s time=%"PRIu64" sec\n",objTypeToString(m_obj->type()),m_startTime);
	int retval = m_obj->attrStopLog( m_attrName );
	return retval;
}

int DeviceStat::stopGrp( ) {
	DBGX("\n");
	for ( unsigned i = 0; i < m_grp->size(); i++ ) {
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
	DBGX("time=%"PRIu64" sec\n",m_stopTime);
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

int DeviceStat::getValue( double* value, PWR_TimePeriod* statTimes ) {

	return objGetValue( m_obj, value, statTimes );
}

int DeviceStat::objGetValue( Object* obj, double* value,
								PWR_TimePeriod* statTimes )
{
    if ( PWR_TIME_UNINIT == statTimes->start ) {
        statTimes->start = m_startTime; 
    } 
    if ( PWR_TIME_UNINIT == statTimes->stop ) {
        if ( m_isLogging ) { 
            statTimes->stop = getTime(); 
        } else {
            statTimes->stop = m_stopTime; 
        } 
    } 
	double windowTime = statTimes->stop - statTimes->start;	
	windowTime /= 1000000000;

	DBGX( "start=%lf stop=%lf\n", (double)statTimes->start/1000000000,
				(double)statTimes->stop/1000000000); 

	unsigned int nSamples = windowTime / m_period;  

	DBGX("wndwLngth=%.9f smpls=%d\n", windowTime, nSamples);

	std::vector<double> values(nSamples,0.0);
	PWR_Time timeStamp;
	int retval = obj->attrGetSamples( m_attrName, &timeStamp, 
								m_period, &nSamples, &values[0] );


	statTimes->start = timeStamp;
	statTimes->stop = timeStamp + nSamples * m_period * 1000000000 ;
	DBGX("actual: start=%lf stop=%lf count=%"PRIu32"\n", 
			(double) timeStamp/1000000000, 
			(double) statTimes->stop/1000000000, nSamples);	

	statTimes->instant = PWR_TIME_UNINIT;
	int pos;
	*value = opPtr( values, pos );
    if ( pos > -1 ) {
        statTimes->instant = statTimes->start + pos * m_period * 1000000000; 
    }

	DBGX("actual: start=%lf stop=%lf instant=%lf count=%"PRIu32"\n", 
			(double) timeStamp/1000000000, 
			(double) statTimes->stop/1000000000, 
            (double) statTimes->instant/1000000000,nSamples);	

	return retval;
}

int DeviceStat::getValues( double value[], PWR_TimePeriod statTimes[] ) 
{
	DBGX("\n");
	for ( unsigned i = 0; i < m_grp->size(); i++ ) {
		int retval  = objGetValue( m_grp->getObj(i), &value[i], &statTimes[i] );
		if ( retval != PWR_RET_SUCCESS ) {
			return retval;
		} 	
	}
	return PWR_RET_SUCCESS;
}

