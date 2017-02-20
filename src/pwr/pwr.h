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

#ifndef _PWR_H 
#define _PWR_H 

#include <time.h>

#include "pwrtypes.h"

#ifdef __cplusplus
class EventChannel;
extern "C" {
#else
typedef void EventChannel;
#endif

/*
 * Subset of API for initialization
 */

int         PWR_CntxtInit( PWR_CntxtType, PWR_Role, const char* name, PWR_Cntxt* );
int         PWR_CntxtDestroy( PWR_Cntxt );

/*
 * Subset of API for navigation
 */

int         PWR_CntxtGetEntryPoint( PWR_Cntxt, PWR_Obj* );
int         PWR_CntxtGetObjByName( PWR_Cntxt, const char* name, PWR_Obj* );

int         PWR_ObjGetType( PWR_Obj, PWR_ObjType* );
int         PWR_ObjGetName( PWR_Obj, char* dest, size_t len );
int         PWR_ObjGetParent( PWR_Obj, PWR_Obj* parent );
int         PWR_ObjGetChildren( PWR_Obj, PWR_Grp* );


/*
 * Subset of API for groups
 */

int         PWR_GrpCreate( PWR_Cntxt, PWR_Grp* );
int         PWR_GrpDestroy( PWR_Grp );
int         PWR_GrpDuplicate( PWR_Grp, PWR_Grp* dup );

int         PWR_GrpUnion( PWR_Grp, PWR_Grp, PWR_Grp* result );
int         PWR_GrpIntersection( PWR_Grp, PWR_Grp, PWR_Grp* result );
int         PWR_GrpDifference( PWR_Grp, PWR_Grp, PWR_Grp* result );

int         PWR_GrpGetNumObjs( PWR_Grp );
int         PWR_GrpGetObjByIndx( PWR_Grp, int, PWR_Obj* );
int         PWR_GrpAddObj( PWR_Grp, PWR_Obj );
int         PWR_GrpRemoveObj( PWR_Grp, PWR_Obj );

int         PWR_CntxtGetGrpByName( PWR_Cntxt, const char* name, PWR_Grp* );

/*
 * Subset of API for attributes
 */

int         PWR_ObjAttrGetValue( PWR_Obj, PWR_AttrName name, void* val, PWR_Time* );
int         PWR_ObjAttrSetValue( PWR_Obj, PWR_AttrName name, const void* val );

int         PWR_StatusCreate( PWR_Cntxt, PWR_Status* );
int         PWR_StatusDestroy( PWR_Status );
int         PWR_StatusPopError( PWR_Status, PWR_AttrAccessError* );
int         PWR_StatusClear( PWR_Status );

int         PWR_ObjAttrGetValues( PWR_Obj, int count, PWR_AttrName names[], void* vals, PWR_Time ts[], PWR_Status );
int         PWR_ObjAttrSetValues( PWR_Obj, int count, PWR_AttrName names[], void* vals, PWR_Status );

int         PWR_ObjAttrIsValid( PWR_Obj, PWR_AttrName );

int         PWR_GrpAttrGetValue( PWR_Grp, PWR_AttrName, void* vals, PWR_Time ts[], PWR_Status );
int         PWR_GrpAttrSetValue( PWR_Grp, PWR_AttrName, void* val, PWR_Status );

int         PWR_GrpAttrGetValues( PWR_Grp, int count, PWR_AttrName[], void* vals, PWR_Time ts[], PWR_Status );
int         PWR_GrpAttrSetValues( PWR_Grp, int count, PWR_AttrName[], void* vals, PWR_Status );

/*
 * Subset of API for metadata
 */

int         PWR_ObjAttrGetMeta( PWR_Obj, PWR_AttrName, PWR_MetaName, void* val );
int         PWR_ObjAttrSetMeta( PWR_Obj, PWR_AttrName, PWR_MetaName, const void* val );
int         PWR_MetaValueAtIndex( PWR_Obj, PWR_AttrName, unsigned int index, void* val, char* val_str );

/*
 * Subset of API for statistics
 */

int			PWR_ObjGetStat( PWR_Obj, PWR_AttrName, PWR_AttrStat, PWR_TimePeriod*, double* );
int			PWR_GrpGetStats( PWR_Grp, PWR_AttrName, PWR_AttrStat, PWR_TimePeriod*, double [],
	   				PWR_TimePeriod []);


int         PWR_ObjCreateStat( PWR_Obj, PWR_AttrName, PWR_AttrStat, PWR_Stat* );
int         PWR_GrpCreateStat( PWR_Grp, PWR_AttrName, PWR_AttrStat, PWR_Stat* );

int         PWR_StatStart( PWR_Stat );
int         PWR_StatStop( PWR_Stat );
int         PWR_StatClear( PWR_Stat );

int         PWR_StatGetValue( PWR_Stat, double* val, PWR_TimePeriod* );
int         PWR_StatGetValues( PWR_Stat, double vals[], PWR_TimePeriod[] );

int         PWR_StatGetReduce( PWR_Stat, PWR_AttrStat, int* index, double* val, PWR_TimePeriod* );

int         PWR_GrpGetReduce( PWR_Grp, PWR_AttrName, PWR_AttrStat, PWR_AttrStat, PWR_TimePeriod, 
								int* index, double* val, PWR_TimePeriod* );

int         PWR_StatDestroy( PWR_Stat );

/* Subset of API for version info
 *
 */

int         PWR_GetMajorVersion( );
int         PWR_GetMinorVersion( );

/*
 * Subset of API for report functions
 */

int         PWR_GetReportByID( PWR_Cntxt, const char* id, PWR_ID, PWR_AttrName, PWR_AttrStat, double*, PWR_TimePeriod* );

/*
 * Subset of API for role / system interfaces
 */

int         PWR_StateTransitDelay( PWR_Obj, PWR_OperState, PWR_OperState, PWR_Time* );
int         PWR_AppTuningHint( PWR_Obj, PWR_RegionHint, PWR_RegionIntensity );
int         PWR_SetSleepStateLimit( PWR_Obj, PWR_SleepState );
int         PWR_WakeUpLatency( PWR_Obj, PWR_SleepState, PWR_Time );
int         PWR_RecommendSleepState( PWR_Obj, PWR_Time, PWR_SleepState* );
int         PWR_SetPerfState( PWR_Obj, PWR_PerfState );
int         PWR_GetPerfState( PWR_Obj, PWR_PerfState* );
int         PWR_GetSleepState( PWR_Obj, PWR_SleepState* );

/* Extensions not in the specifications */

PWR_Grp PWR_ObjGetDescendantsByType( PWR_Obj, PWR_ObjType );
PWR_Obj PWR_ObjGetAncestorByType( PWR_Obj, PWR_ObjType );

EventChannel* PWR_CntxtGetEventChannel( PWR_Cntxt ctx );
int PWR_CntxtMakeProgress( PWR_Cntxt ctx );

PWR_Request PWR_ReqCreate( PWR_Cntxt, PWR_Status );
PWR_Request PWR_ReqCreateCallback( PWR_Cntxt, PWR_Status, Callback callback,
										void* data );
int PWR_ReqDestroy( PWR_Request );

int PWR_ReqWait( PWR_Request );

int PWR_ObjAttrGetValues_NB( PWR_Obj, int count, PWR_AttrName name[],
								void* buf, PWR_Time [], PWR_Request );

int PWR_ObjAttrSetValues_NB( PWR_Obj, int count, PWR_AttrName name[],
								void* buf, PWR_Request );
/*
*  Utility Functions
*/

const char* PWR_ObjGetTypeString( PWR_ObjType );
const char* PWR_AttrGetTypeString( PWR_AttrName );

int PWR_TimeConvert( PWR_Time time, time_t* );

int PWR_ObjAttrStartLog( PWR_Obj, PWR_AttrName name );
int PWR_ObjAttrStopLog( PWR_Obj, PWR_AttrName name );
int PWR_ObjAttrGetSamples( PWR_Obj, PWR_AttrName name, PWR_Time* start,
				double period, unsigned int count, void* buf );

int PWR_ObjAttrStartLog_NB( PWR_Obj, PWR_AttrName name, PWR_Request );
int PWR_ObjAttrStopLog_NB( PWR_Obj, PWR_AttrName name, PWR_Request );
int PWR_ObjAttrGetSamples_NB( PWR_Obj, PWR_AttrName name, PWR_Time* start,
				double period, unsigned int* count, void* buf,  PWR_Request );


int PWR_GrpFilterByAttrName( PWR_Grp, PWR_AttrName, PWR_Grp* );
int PWR_GrpFilterByObjType( PWR_Grp, PWR_ObjType, PWR_Grp* );
int PWR_CntxtGetGrpByType( PWR_Cntxt, PWR_ObjType, PWR_Grp* );

#ifdef __cplusplus
}
#endif

#endif
