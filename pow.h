#ifndef PWR_H 
#define PWR_H 

#include "./types.h"

#ifdef __cplusplus
extern "C" {
#endif

PWR_Cntxt PWR_CntxtInit( PWR_CntxtType, const char* name );
int       PWR_CntxtDestroy( PWR_Cntxt );
PWR_Obj   PWR_CntxtGetSelf( PWR_Cntxt );
PWR_Grp   PWR_CntxtGetGrpByType( PWR_Cntxt, PWR_ObjType );
PWR_Grp   PWR_CntxtGetGrpByName( PWR_Cntxt, const char* name );
PWR_Grp   PWR_CntxtCreateGrp( PWR_Cntxt, const char* name );

PWR_CntxtState PWR_CntxtSave( PWR_Cntxt );
int            PWR_CntxtRestore( PWR_Cntxt, PWR_CntxtState );

const char* PWR_ObjGetName( PWR_Obj );
PWR_ObjType PWR_ObjGetType( PWR_Obj );
PWR_Obj     PWR_ObjGetParent( PWR_Obj );
PWR_Grp     PWR_ObjGetChildren( PWR_Obj );
PWR_Attr    PWR_ObjGetAttrByType( PWR_Obj, PWR_AttrType );
int         PWR_ObjGetNumAttrs( PWR_Obj );
PWR_Attr    PWR_ObjGetAttrByIndx( PWR_Obj, int index );

const char* PWR_ObjGetTypeString( PWR_ObjType );

// note that PWR_GrpDestroy() pairs with PWR_CntxtCreateGrp() but it 
// does not take a context as an argument, should it? 
int         PWR_GrpDestroy( PWR_Grp );
const char* PWR_GrpGetName( PWR_Grp );
int         PWR_GrpGetNumObjs( PWR_Grp );
PWR_Obj     PWR_GrpGetObjByIndx( PWR_Grp, int );
int         PWR_GrpAddObj( PWR_Grp, PWR_Obj );
int         PWR_GrpRemoveObj( PWR_Grp, PWR_Obj );

int     PWR_GrpAttrFloatSetValue( PWR_Attr, float, PWR_Grp );
int     PWR_GrpAttrIntSetValue( PWR_Attr, int, PWR_Grp );
int     PWR_GrpAttrStringSetValue( PWR_Attr, const char*, PWR_Grp );

PWR_AttrType      PWR_AttrGetType( PWR_Attr );
PWR_AttrValueType PWR_AttrGetValueType( PWR_Attr );
int               PWR_AttrGetUnits( PWR_Attr, PWR_AttrUnits* );

const char* PWR_AttrGetTypeString( PWR_AttrType );

int PWR_AttrFloatGetMin( PWR_Attr, float* );
int PWR_AttrFloatGetMax( PWR_Attr, float* );
int PWR_AttrFloatGetValue( PWR_Attr, float* );
int PWR_AttrFloatSetValue( PWR_Attr, float );


int PWR_AttrIntGetMin( PWR_Attr, int* );
int PWR_AttrIntGetMax( PWR_Attr, int* );
int PWR_AttrIntGetValue( PWR_Attr, int* );
int PWR_AttrIntSetValue( PWR_Attr, int );

int PWR_AttrStringGetPossible( PWR_Attr, char*, int strlen ); 
int PWR_AttrStringGetValue( PWR_Attr, char*, int strlen );
int PWR_AttrStringSetValue( PWR_Attr, const char* );

#ifdef __cplusplus
}
#endif

#endif
