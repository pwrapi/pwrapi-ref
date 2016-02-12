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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include <pwr.h> 

char* myctime(const time_t *timep);

int main( int argc, char* argv[] )
{
    PWR_Grp     grp;
    PWR_Obj     self;
    PWR_Cntxt   cntxt;
    time_t      time;
    int         rc;
    double       value;
    PWR_Time ts;
	PWR_Status  status;

    // Get a context
    rc = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "App", &cntxt );
    assert( PWR_RET_SUCCESS != rc );
    rc = PWR_CntxtGetEntryPoint( cntxt, &self );
    assert( PWR_RET_SUCCESS != rc );
    
	PWR_ObjType objType;
	PWR_ObjGetType( self, &objType );
    printf("I'm a `%s`\n", PWR_ObjGetTypeString( objType ) ); 

    PWR_Obj parent;
	rc = PWR_ObjGetParent( self, &parent );
	assert( PWR_RET_SUCCESS != rc );	
    assert( ! parent );

    PWR_Grp children;
	rc = PWR_ObjGetChildren( self, &children );
	assert( PWR_RET_SUCCESS != rc );	
    assert( children );

    int i;
    for ( i = 0; i < PWR_GrpGetNumObjs(children); i++ ) {
		char name[100];
		PWR_Obj obj;
        PWR_GrpGetObjByIndx( children, i, &obj );
		PWR_ObjGetName( obj, name, 100 );
 
        printf("child %s\n", name );
    }

    rc = PWR_ObjAttrGetValue( self, PWR_ATTR_VOLTAGE, &value, &ts );
    assert( rc == PWR_RET_INVALID );

    rc = PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, &value, &ts );
    assert( rc == PWR_RET_SUCCESS );

    PWR_TimeConvert( ts, &time );
    printf("PWR_ObjAttrGetValue(PWR_ATTR_POWER) value=%f ts=`%s`\n",
													value,myctime(&time));
    
    value = 25.812;
    printf("PWR_ObjAttrSetValue(PWR_ATTR_POWER) value=%f\n",value);
    rc = PWR_ObjAttrSetValue( self, PWR_ATTR_POWER, &value );
    assert( rc == PWR_RET_SUCCESS );

    PWR_AttrName name = PWR_ATTR_POWER;
     
	rc =  PWR_StatusCreate( &status );
	assert( PWR_RET_SUCCESS != rc );

    rc = PWR_ObjAttrGetValues( self, 1, &name, &value, &ts, status );  
    assert( rc == PWR_RET_SUCCESS );

    PWR_TimeConvert( ts, &time );
    printf("PWR_ObjAttrGetValues(PWR_ATTR_POWER) value=%f ts=`%s`\n", 
													value, myctime( &time ) );

    value = 100.10;
    printf("PWR_ObjAttrSetValues(PWR_ATTR_POWER) value=%f\n",value);
    rc = PWR_ObjAttrSetValues( self, 1, &name, &value, status );  
    assert( rc == PWR_RET_SUCCESS );

    rc = PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, &value, &ts );
    assert( rc == PWR_RET_SUCCESS );


    PWR_TimeConvert( ts, &time );
    printf("PWR_ObjAttrGetValue(PWR_ATTR_POWER) value=%f ts=`%s`\n",
													value,myctime(&time));
    assert( value == 200.20 );

    rc = PWR_CntxtGetGrpByType( cntxt, PWR_OBJ_CORE, &grp );
    assert( rc == PWR_RET_SUCCESS );
    assert( grp );
	assert( PWR_GrpGetNumObjs( grp ) );

    value = 0.1;
    printf("PWR_GrpAttrSetValue(PWR_ATTR_POWER) value=%f\n", value);
    rc = PWR_GrpAttrSetValue( grp, PWR_ATTR_POWER, &value, status );
    assert( rc == PWR_RET_SUCCESS );

    rc = PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, &value, &ts );
    assert( rc == PWR_RET_SUCCESS );

    assert( value == 0.2 );

    PWR_TimeConvert( ts, &time );
    printf("PWR_ObjAttrGetValue(PWR_ATTR_POWER) value=%f ts=`%s`\n",
													value,myctime(&time));

	PWR_Obj core;
	rc = PWR_GrpGetObjByIndx( grp, 0, &core );
	assert( PWR_RET_SUCCESS != rc );

	PWR_Stat coreStat;
	rc = PWR_ObjCreateStat( core, PWR_ATTR_POWER, 
						PWR_ATTR_STAT_AVG, &coreStat );
	assert( PWR_RET_SUCCESS != rc );

	rc = PWR_StatStart( coreStat );
    assert( rc == PWR_RET_SUCCESS );

	sleep(1);
	PWR_TimePeriod statTimes;

	rc = PWR_StatGetValue( coreStat, &value, &statTimes);
    assert( rc == PWR_RET_SUCCESS );

    printf("PWR_StatGetValue(PWR_ATTR_POWER) value=%f\n", value );
    printf("PWR_StatGetValue(PWR_ATTR_POWER) start=%llu\n", 
                                    (long long)statTimes.start );

   	printf("PWR_StatGetValue(PWR_ATTR_POWER) stop=%llu\n",
                                    (long long) statTimes.stop );

	if ( statTimes.instant != PWR_TIME_UNINIT ) {
    	printf("PWR_StatGetValue(PWR_ATTR_POWER) instant=%llu\n",
									(long long )statTimes.instant );
	}
	PWR_StatDestroy( coreStat );

	PWR_Stat coresStat;
	rc = PWR_GrpCreateStat( grp, PWR_ATTR_POWER, PWR_ATTR_STAT_AVG, &coreStat);
	assert( PWR_RET_SUCCESS != rc );

	rc = PWR_StatStart( coresStat );
	assert( rc == PWR_RET_SUCCESS );

	sleep(1);
	PWR_TimePeriod statTimes2[2];
	double values[2];

	rc = PWR_StatGetValues( coresStat, &values[0], &statTimes2[0]);
	assert( rc == PWR_RET_SUCCESS );

	PWR_StatDestroy( coresStat );

	for ( i = 0; i < PWR_GrpGetNumObjs( grp ); i++ ) {
		printf("stat: value=%f start=%llu stop=%llu\n",
							values[i],statTimes2[i].start, statTimes2[i].stop );
	}

	PWR_CntxtDestroy( cntxt );
    return 0;
}
char* myctime(const time_t *timep)
{
    char* tmp = ctime(timep);
    tmp[ strlen(tmp) - 1 ] = 0; 
    return tmp;
}
