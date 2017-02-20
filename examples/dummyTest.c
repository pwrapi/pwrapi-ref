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
    assert( PWR_RET_SUCCESS == rc );

    rc = PWR_CntxtGetEntryPoint( cntxt, &self );
    assert( PWR_RET_SUCCESS == rc );
    
	PWR_ObjType objType;
	PWR_ObjGetType( self, &objType );
    printf("I am a `%s`\n", PWR_ObjGetTypeString( objType ) ); 

    PWR_Obj parent;
	rc = PWR_ObjGetParent( self, &parent );
	assert( rc >= PWR_RET_SUCCESS );	

    PWR_Grp children;
	rc = PWR_ObjGetChildren( self, &children );
	assert( rc >= PWR_RET_SUCCESS );	

    int i;
    for ( i = 0; i < PWR_GrpGetNumObjs(children); i++ ) {
		char name[100];
		PWR_Obj obj;
        PWR_GrpGetObjByIndx( children, i, &obj );
		PWR_ObjGetName( obj, name, 100 );
 
        printf("child %s\n", name );
    }

    rc = PWR_ObjAttrGetValue( self, PWR_ATTR_VOLTAGE, &value, &ts );
    assert( PWR_RET_INVALID == rc );

    rc = PWR_ObjAttrGetValue( self, PWR_ATTR_ENERGY, &value, &ts );
    assert( PWR_RET_SUCCESS == rc );

    PWR_TimeConvert( ts, &time );
    printf("PWR_ObjAttrGetValue(PWR_ATTR_ENERGY) value=%f ts=`%s`\n",
													value,myctime(&time));
    
    value = 25.812;
    printf("PWR_ObjAttrSetValue(PWR_ATTR_ENERGY) value=%f\n",value);
    rc = PWR_ObjAttrSetValue( self, PWR_ATTR_ENERGY, &value );
    assert( PWR_RET_SUCCESS == rc );

    PWR_AttrName name = PWR_ATTR_ENERGY;
     
	rc =  PWR_StatusCreate( cntxt, &status );
	assert( PWR_RET_SUCCESS == rc );

    rc = PWR_ObjAttrGetValues( self, 1, &name, &value, &ts, status );  
    assert( PWR_RET_SUCCESS == rc );

    PWR_TimeConvert( ts, &time );
    printf("PWR_ObjAttrGetValues(PWR_ATTR_ENERGY) value=%f ts=`%s`\n", 
													value, myctime( &time ) );

    value = 100.10;
    printf("PWR_ObjAttrSetValues(PWR_ATTR_ENERGY) value=%f\n",value);
    rc = PWR_ObjAttrSetValues( self, 1, &name, &value, status );  
    assert( PWR_RET_SUCCESS == rc );

    rc = PWR_ObjAttrGetValue( self, PWR_ATTR_ENERGY, &value, &ts );
    assert( PWR_RET_SUCCESS == rc );


    PWR_TimeConvert( ts, &time );
    printf("PWR_ObjAttrGetValue(PWR_ATTR_ENERGY) value=%f ts=`%s`\n",
													value,myctime(&time));

    rc = PWR_CntxtGetGrpByType( cntxt, PWR_OBJ_CORE, &grp );
    assert ( PWR_RET_SUCCESS == rc );

    value = 0.1;
    printf("PWR_GrpAttrSetValue(PWR_ATTR_ENERGY) value=%f\n", value);
    rc = PWR_GrpAttrSetValue( grp, PWR_ATTR_ENERGY, &value, status );
    assert( PWR_RET_SUCCESS == rc );

    rc = PWR_ObjAttrGetValue( self, PWR_ATTR_ENERGY, &value, &ts );
    assert( PWR_RET_SUCCESS == rc );

    PWR_TimeConvert( ts, &time );
    printf("PWR_ObjAttrGetValue(PWR_ATTR_ENERGY) value=%f ts=`%s`\n",
													value,myctime(&time));

    PWR_Obj node;
    rc = PWR_GrpGetObjByIndx( grp, 0, &node );
    assert( PWR_RET_SUCCESS == rc );

	PWR_Stat nodeStat;
	rc = PWR_ObjCreateStat( node, PWR_ATTR_ENERGY, 
						PWR_ATTR_STAT_AVG, &nodeStat );
	assert( PWR_RET_SUCCESS == rc );

	rc = PWR_StatStart( nodeStat );
    assert( PWR_RET_SUCCESS == rc );

	sleep(1);
	PWR_TimePeriod statTimes;
    statTimes.start = statTimes.stop = PWR_TIME_UNINIT;

	rc = PWR_StatGetValue( nodeStat, &value, &statTimes );
    assert( PWR_RET_SUCCESS == rc );

    printf("PWR_StatGetValue(PWR_ATTR_ENERGY) value=%lf\n", value );
    printf("PWR_StatGetValue(PWR_ATTR_ENERGY) start=%lf\n", 
                                    (double)statTimes.start / 1000000000 );

   	printf("PWR_StatGetValue(PWR_ATTR_ENERGY) stop=%lf\n",
                                    (double) statTimes.stop / 1000000000);

	if ( statTimes.instant != PWR_TIME_UNINIT ) {
    	printf("PWR_StatGetValue(PWR_ATTR_ENERGY) instant=%lf\n",
									(double)statTimes.instant / 1000000000 );
	}
	PWR_StatDestroy( nodeStat );

	PWR_Stat nodesStat;
	rc = PWR_GrpCreateStat( grp, PWR_ATTR_ENERGY, PWR_ATTR_STAT_AVG, &nodesStat);
	assert( PWR_RET_SUCCESS == rc );

    int grpSize = PWR_GrpGetNumObjs( grp ); 
	PWR_TimePeriod* statTimes2 = malloc( grpSize * sizeof(*statTimes2) );
	double* values = malloc( grpSize * sizeof(*values) );

	for ( i = 0; i < grpSize; i++ ) {
        statTimes2[i].start = statTimes2[i].stop = PWR_TIME_UNINIT;
    } 
	rc = PWR_StatStart( nodesStat );
	assert( PWR_RET_SUCCESS == rc);

	sleep(1);

	rc = PWR_StatGetValues( nodesStat, &values[0], &statTimes2[0]);
	assert( PWR_RET_SUCCESS == rc );

	PWR_StatDestroy( nodesStat );

	for ( i = 0; i < grpSize; i++ ) {
		printf("stat: value=%lf start=%lf stop=%lf\n", values[i],
            (double) statTimes2[i].start/1000000000, 
            (double)statTimes2[i].stop/1000000000 );
	}

	//PWR_CntxtDestroy( cntxt );
    return 0;
}
char* myctime(const time_t *timep)
{
    char* tmp = ctime(timep);
    tmp[ strlen(tmp) - 1 ] = 0; 
    return tmp;
}
