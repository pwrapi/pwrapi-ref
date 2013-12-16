#ifndef PLI_H 
#define PLI_H 

#include "./types.h"

typedef struct _Context*    PLI_Context;
typedef struct _Group*      PLI_Group;
typedef struct _Object*     PLI_Object;
typedef struct _Attribute*  PLI_Attribute;

#ifdef __cplusplus
extern "C" {
#endif

PLI_Context PLI_ContextInit( PLI_ContextType );
int         PLI_ContextDestroy( PLI_Context );
PLI_Object  PLI_ContextGetSelf( PLI_Context );
PLI_Group   PLI_ContextGetGroupByType( PLI_Context, PLI_ObjectType );
PLI_Group   PLI_ContextGetGroupByName( PLI_Context, const char* name );
PLI_Group   PLI_ContextCreateGroup( PLI_Context, const char* name );

const char*    PLI_ObjectGetName( PLI_Object );
PLI_ObjectType PLI_ObjectGetType( PLI_Object );
PLI_Object     PLI_ObjectGetParent( PLI_Object );
PLI_Group      PLI_ObjectGetChildren( PLI_Object );
PLI_Attribute  PLI_ObjectGetAttributeByName( PLI_Object, const char* name );
int            PLI_ObjectGetNumAttributes( PLI_Object );
PLI_Attribute  PLI_ObjectGetAttributeByIndex( PLI_Object, int index );
int            PLI_ObjectCompareType( PLI_ObjectType, PLI_ObjectType );

// note that PLI_GroupDestroy() pairs with PLI_ContextCreateGroup() but it 
// does not take a context as an argument, should it? 
int         PLI_GroupDestroy( PLI_Group );
const char* PLI_GroupGetName( PLI_Group );
int         PLI_GroupGetNumObjects( PLI_Group );
PLI_Object  PLI_GroupGetObjectByIndex( PLI_Group, int );
int         PLI_GroupAddObject( PLI_Group, PLI_Object );
int         PLI_GroupRemoveObject( PLI_Group, PLI_Object );

const char*       PLI_AttributeGetName( PLI_Attribute );
PLI_AttributeType PLI_AttributeGetType( PLI_Attribute );
int               PLI_AttributeGetScale( PLI_Attribute, PLI_AttributeScale* );
int              PLI_AttributeCompareType(PLI_AttributeType, PLI_AttributeType);

int PLI_AttributeFloatGetMin( PLI_Attribute, float* );
int PLI_AttributeFloatGetMax( PLI_Attribute, float* );
int PLI_AttributeFloatGetValue( PLI_Attribute, float* );
int PLI_AttributeFloatSetValue( PLI_Attribute, float );

int PLI_AttributeIntGetMin( PLI_Attribute, int* );
int PLI_AttributeIntGetMax( PLI_Attribute, int* );
int PLI_AttributeIntGetValue( PLI_Attribute, int* );
int PLI_AttributeIntSetValue( PLI_Attribute, int );

int PLI_AttributeStringGetPossible( PLI_Attribute, char*, int strlen ); 
int PLI_AttributeStringGetValue( PLI_Attribute, char*, int strlen );
int PLI_AttributeStringSetValue( PLI_Attribute, const char* );

#ifdef __cplusplus
}
#endif

#endif
