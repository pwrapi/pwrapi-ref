#ifndef PLI_H 
#define PLI_H 

#include "./types.h"

#ifdef __cplusplus
extern "C" {
#endif

PLI_Cntxt PLI_CntxtInit( PLI_CntxtType, const char* name );
int       PLI_CntxtDestroy( PLI_Cntxt );
PLI_Obj   PLI_CntxtGetSelf( PLI_Cntxt );
PLI_Grp   PLI_CntxtGetGrpByType( PLI_Cntxt, PLI_ObjType );
PLI_Grp   PLI_CntxtGetGrpByName( PLI_Cntxt, const char* name );
PLI_Grp   PLI_CntxtCreateGrp( PLI_Cntxt, const char* name );

PLI_CntxtState PLI_CntxtSave( PLI_Cntxt );
int            PLI_CntxtRestore( PLI_Cntxt, PLI_CntxtState );

const char* PLI_ObjGetName( PLI_Obj );
PLI_ObjType PLI_ObjGetType( PLI_Obj );
PLI_Obj     PLI_ObjGetParent( PLI_Obj );
PLI_Grp     PLI_ObjGetChildren( PLI_Obj );
PLI_Attr    PLI_ObjGetAttrByName( PLI_Obj, const char* name );
int         PLI_ObjGetNumAttrs( PLI_Obj );
PLI_Attr    PLI_ObjGetAttrByIndx( PLI_Obj, int index );
int         PLI_ObjCompareType( PLI_ObjType, PLI_ObjType );

// note that PLI_GrpDestroy() pairs with PLI_CntxtCreateGrp() but it 
// does not take a context as an argument, should it? 
int         PLI_GrpDestroy( PLI_Grp );
const char* PLI_GrpGetName( PLI_Grp );
int         PLI_GrpGetNumObjs( PLI_Grp );
PLI_Obj     PLI_GrpGetObjByIndx( PLI_Grp, int );
int         PLI_GrpAddObj( PLI_Grp, PLI_Obj );
int         PLI_GrpRemoveObj( PLI_Grp, PLI_Obj );

PLI_Grp     PLI_GrpAttrFloatSetValue( PLI_Attr, float );
PLI_Grp     PLI_GrpAttrIntSetValue( PLI_Attr, int );
PLI_Grp     PLI_GrpAttrStringSetValue( PLI_Attr, const char* );

const char*  PLI_AttrGetName( PLI_Attr );
PLI_AttrType PLI_AttrGetType( PLI_Attr );
int          PLI_AttrGetScale( PLI_Attr, PLI_AttrScale* );
int          PLI_AttrCompareType(PLI_AttrType, PLI_AttrType);

int PLI_AttrFloatGetMin( PLI_Attr, float* );
int PLI_AttrFloatGetMax( PLI_Attr, float* );
int PLI_AttrFloatGetValue( PLI_Attr, float* );
int PLI_AttrFloatSetValue( PLI_Attr, float );


int PLI_AttrIntGetMin( PLI_Attr, int* );
int PLI_AttrIntGetMax( PLI_Attr, int* );
int PLI_AttrIntGetValue( PLI_Attr, int* );
int PLI_AttrIntSetValue( PLI_Attr, int );

int PLI_AttrStringGetPossible( PLI_Attr, char*, int strlen ); 
int PLI_AttrStringGetValue( PLI_Attr, char*, int strlen );
int PLI_AttrStringSetValue( PLI_Attr, const char* );

#ifdef __cplusplus
}
#endif

#endif
