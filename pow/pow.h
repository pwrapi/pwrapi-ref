/* 
 * Copyright 2014 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 */

#ifndef _PWR_H 
#define _PWR_H 

#include <time.h>

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

int PWR_GetMajorVersion();
int PWR_GetMinorVersion();

/*
* Subset of API that works on Cntxt
*/

PWR_Cntxt PWR_CntxtInit( PWR_CntxtType, PWR_Role, const char* name );
int       PWR_CntxtDestroy( PWR_Cntxt );
PWR_Obj   PWR_CntxtGetEntryPoint( PWR_Cntxt );
PWR_Grp   PWR_CntxtGetGrpByType( PWR_Cntxt, PWR_ObjType );
PWR_Grp   PWR_CntxtGetGrpByName( PWR_Cntxt, const char* name );

PWR_CntxtState PWR_CntxtSave( PWR_Cntxt );
int            PWR_CntxtRestore( PWR_Cntxt, PWR_CntxtState );


PWR_Obj PWR_CntxtGetObjByName( PWR_Cntxt, const char* name );

/*
* Subset of API that works on Obj 
*/

PWR_ObjType PWR_ObjGetType( PWR_Obj );
const char* PWR_ObjGetName( PWR_Obj );
PWR_Obj     PWR_ObjGetParent( PWR_Obj );
PWR_Grp     PWR_ObjGetChildren( PWR_Obj );

int PWR_ObjAttrIsValid( PWR_Obj, PWR_AttrName name );

int PWR_ObjAttrGetValue( PWR_Obj, PWR_AttrName name, void* buf, PWR_Time * );
int PWR_ObjAttrSetValue( PWR_Obj, PWR_AttrName name, void* buf );

int PWR_ObjAttrGetValues( PWR_Obj, int count, PWR_AttrName names[], void* buf, PWR_Time ts[], PWR_Status  );
int PWR_ObjAttrSetValues( PWR_Obj, int count, PWR_AttrName names[], void* buf, PWR_Status );

int PWR_ObjGetAvgPower( PWR_Obj, PWR_Time* from, PWR_Time* to );
int PWR_ObjGetEnergy( PWR_Obj, PWR_Time* from, PWR_Time* to );

/*
* Subset of API that works on Grp 
*/

// note that PWR_GrpDestroy() pairs with PWR_CntxtCreateGrp() but it 
// does not take a context as an argument, should it? 

PWR_Grp     PWR_GrpCreate( PWR_Cntxt, const char* name );
int         PWR_GrpDestroy( PWR_Grp );
const char* PWR_GrpGetName( PWR_Grp );
int         PWR_GrpGetNumObjs( PWR_Grp );
PWR_Obj     PWR_GrpGetObjByIndx( PWR_Grp, int );
int         PWR_GrpAddObj( PWR_Grp, PWR_Obj );
int         PWR_GrpRemoveObj( PWR_Grp, PWR_Obj );

int         PWR_GrpAttrSetValue( PWR_Grp, PWR_AttrName name, void* buf, PWR_Status );
int         PWR_GrpAttrGetValue( PWR_Grp, PWR_AttrName name, void* buf, PWR_Time ts[], PWR_Status );

int         PWR_GrpAttrSetValues( PWR_Grp, int count, PWR_AttrName names[], void* buf, PWR_Status );
int         PWR_GrpAttrGetValues( PWR_Grp, int count, PWR_AttrName names[], void* buf, PWR_Time ts[], PWR_Status );

PWR_Status PWR_StatusCreate();
int PWR_StatusDestroy( PWR_Status );
int PWR_StatusPopError( PWR_Status, PWR_AttrAccessError* );
int PWR_StatusClear( PWR_Status );

/*
 * Statistics
 */


PWR_Stat PWR_ObjCreateStat( PWR_Obj, PWR_AttrName name, PWR_AttrStat stat );
PWR_Stat PWR_GrpCreateStat( PWR_Grp, PWR_AttrName name, PWR_AttrStat stat );

int PWR_StatDestroy( PWR_Stat );

int PWR_StatStart( PWR_Stat );
int PWR_StatStop( PWR_Stat );
int PWR_StatClear( PWR_Stat );


int PWR_StatGetValue( PWR_Stat, double* value, PWR_StatTimes* statTimes );
/*
 * size of array passed in is derived by the function
 */
int PWR_StatGetValues( PWR_Stat, double values[],
                                            PWR_StatTimes statTimes[] );

/*
*  Utility Functions
*/

const char* PWR_ObjGetTypeString( PWR_ObjType );
const char* PWR_AttrGetTypeString( PWR_AttrName );
const char* PWR_ObjGetName( PWR_Obj );

int PWR_TimeConvert( PWR_Time time, time_t* );

/*
 * Operating System -> Application Interface
 */
int PWR_AppHint( PWR_Obj, PWR_RegionHint );
#ifdef __cplusplus
}
#endif

#endif
