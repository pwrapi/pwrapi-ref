#ifndef PWR_H 
#define PWR_H 

#include "./types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
* Subset of API that works on Cntxt
*/

PWR_Cntxt PWR_CntxtInit( PWR_CntxtType, const char* name );
int       PWR_CntxtDestroy( PWR_Cntxt );
PWR_Obj   PWR_CntxtGetSelf( PWR_Cntxt );
PWR_Grp   PWR_CntxtGetGrpByType( PWR_Cntxt, PWR_ObjType );
PWR_Grp   PWR_CntxtGetGrpByName( PWR_Cntxt, const char* name );
PWR_Grp   PWR_CntxtCreateGrp( PWR_Cntxt, const char* name );

PWR_CntxtState PWR_CntxtSave( PWR_Cntxt );
int            PWR_CntxtRestore( PWR_Cntxt, PWR_CntxtState );

/*
* Subset of API that works on Obj 
*/

PWR_ObjType PWR_ObjGetType( PWR_Obj );
PWR_Obj     PWR_ObjGetParent( PWR_Obj );
PWR_Grp     PWR_ObjGetChildren( PWR_Obj );

int         PWR_ObjGetNumAttrs( PWR_Obj );
int   	    PWR_ObjGetAttrTypeByIndx( PWR_Obj, int, PWR_AttrType* value );

int         PWR_ObjAttrGetValueType( PWR_Obj, PWR_AttrType,
                                        PWR_AttrValueType* value );
int         PWR_ObjAttrGetUnits( PWR_Obj, PWR_AttrType, PWR_AttrUnits* );

int PWR_ObjAttrGetRange( PWR_Obj, PWR_AttrType, void* min, void* max );
int PWR_ObjAttrGetValue( PWR_Obj, PWR_AttrType, void* );
int PWR_ObjAttrSetValue( PWR_Obj, PWR_AttrType, void* );

int PWR_ObjAttrFloatGetRange( PWR_Obj, PWR_AttrType, float* min, float* max );
int PWR_ObjAttrFloatGetValue( PWR_Obj, PWR_AttrType, float* );
int PWR_ObjAttrFloatSetValue( PWR_Obj, PWR_AttrType, float* );

int PWR_ObjAttrIntGetRange( PWR_Obj, PWR_AttrType, int* min, int* max );
int PWR_ObjAttrIntGetValue( PWR_Obj, PWR_AttrType, int* );
int PWR_ObjAttrIntSetValue( PWR_Obj, PWR_AttrType, int* );

int PWR_ObjAttrStringGetPossible( PWR_Obj, PWR_AttrType, char*, int len );
int PWR_ObjAttrStringGetValue( PWR_Obj, PWR_AttrType, char*, int len );
int PWR_ObjAttrStringSetValue( PWR_Obj, PWR_AttrType, char*, int len );

/*
* Subset of API that works on Grp 
*/

// note that PWR_GrpDestroy() pairs with PWR_CntxtCreateGrp() but it 
// does not take a context as an argument, should it? 
int         PWR_GrpDestroy( PWR_Grp );
const char* PWR_GrpGetName( PWR_Grp );
int         PWR_GrpGetNumObjs( PWR_Grp );
PWR_Obj     PWR_GrpGetObjByIndx( PWR_Grp, int );
int         PWR_GrpAddObj( PWR_Grp, PWR_Obj );
int         PWR_GrpRemoveObj( PWR_Grp, PWR_Obj );

int PWR_GrpAttrSetValue( PWR_Grp, PWR_AttrType, float*, PWR_Grp errOut );

/*
*  Utility Functions
*/

const char* PWR_ObjGetTypeString( PWR_ObjType );
const char* PWR_AttrGetTypeString( PWR_AttrType );

/*
 * Operating System -> Application Interface
 */
int PWR_AppHint( PWR_Obj, PWR_AttrType, PWR_AttrValueType );
#ifdef __cplusplus
}
#endif

#endif
