
#include "./pli.h"
#include "./object.h"
#include "./init.h"

PWR_Cntxt PWR_CntxtInit( PWR_CntxtType type, const char* name )
{
    return PWR::init( type );
}

int PWR_CntxtDestroy( PWR_Cntxt ctx )
{
    return PWR::destroy( ctx );
}

PWR_Obj PWR_CntxtGetSelf( PWR_Cntxt ctx )
{
    return ctx->getSelf();
}

PWR_Grp PWR_CntxtGetGrpByType( PWR_Cntxt ctx, PWR_ObjType type )
{
    return ctx->getGrp( type );
}

PWR_Grp PWR_CntxtCreateGrp( PWR_Cntxt ctx, const char* name )
{
    return ctx->groupCreate( name );
}

PWR_Grp PWR_CntxtGetGrpByName( PWR_Cntxt ctx, const char* name )
{
    return ctx->getGrpByName( name );
}

const char* PWR_ObjGetName( PWR_Obj obj )
{
    return &obj->name()[0];
}

PWR_ObjType PWR_ObjGetType( PWR_Obj obj )
{
    return obj->type();
}

PWR_Obj PWR_ObjGetParent(PWR_Obj obj )
{
    return obj->parent();
    return NULL;
}

PWR_Grp PWR_ObjGetChildren( PWR_Obj obj )
{
    return& obj->children();
}

int PWR_GrpDestroy( PWR_Grp group )
{
    _Cntxt* ctx = group->getCtx();
    return ctx->groupDestroy( group );
}

int PWR_GrpAddObj( PWR_Grp group, PWR_Obj obj )
{
    return group->add( obj );
}

int PWR_GrpObjRemove( PWR_Grp group, PWR_Obj obj )
{
    return group->remove( obj );
}

int PWR_GrpGetNumObjs( PWR_Grp group )
{
    return group->size();
}

PWR_Obj PWR_GrpGetObjByIndx( PWR_Grp group, int i )
{
    return group->getObj( i );
}

PWR_Obj PWR_GrpGetObjByName( PWR_Grp group, int i )
{
    return NULL;
}

int PWR_ObjGetNumAttrs( PWR_Obj obj )
{
    return obj->attrGetNumber();
}


PWR_Attr PWR_ObjGetAttrByIndx( PWR_Obj obj, int index )
{
    return obj->attributeGet( index );
}

PWR_AttrName   PWR_AttrGetName( PWR_Attr attr )
{
    return attr->name();
}

PWR_AttrType PWR_AttrGetType( PWR_Attr attr )
{
    return attr->type();
}

int PWR_AttrGetUnits( PWR_Attr a, PWR_AttrUnits* value )
{
    _AttrNum* attr = dynamic_cast< _AttrNum*>(a);
    if ( attr ) {
        return attr->unit();
    } 
    return PWR_FAILURE;
}

int PWR_AttrFloatGetMin( PWR_Attr attr, float* value )
{
    *value = static_cast< _AttrNumTemplate<float>* >(attr)->min();
    return PWR_SUCCESS;
}

int PWR_AttrFloatGetMax( PWR_Attr attr, float* value )
{
    *value = static_cast< _AttrNumTemplate<float>* >(attr)->max();
    return PWR_SUCCESS; 
}

int PWR_AttrFloatGetValue( PWR_Attr attr, float* value )
{
    *value = static_cast< _AttrNumTemplate<float>* >(attr)->value();
    return PWR_SUCCESS;
}

int PWR_AttrFloatSetValue( PWR_Attr attr, float value )
{
    return static_cast< _AttrNumTemplate<float>* >(attr)->value( value );
}

int PWR_AttrIntGetMin( PWR_Attr attr, int* value )
{
    *value = static_cast< _AttrNumTemplate<int>* >(attr)->min();
    return PWR_SUCCESS;
}

int PWR_AttrIntGetMax( PWR_Attr attr, int* value )
{
    *value = static_cast< _AttrNumTemplate<int>* >(attr)->max();
    return PWR_SUCCESS;
}

int PWR_AttrIntGetValue( PWR_Attr attr, int* value )
{
    *value = static_cast< _AttrNumTemplate<int>* >(attr)->value();
    return PWR_SUCCESS;
}

int PWR_AttrIntSetValue( PWR_Attr attr, int value )
{
    static_cast< _AttrNumTemplate<int>* >(attr)->value( value );
    return PWR_SUCCESS;
}

int PWR_AttrStringGetValue( PWR_Attr attr, char* value, int len )
{
    strcpy( value, &static_cast< _AttrStringTemplate<std::string>* >(attr)->value()[0] );
    return PWR_SUCCESS; 
}

int PWR_AttrStringGetPossible( PWR_Attr attr, char* value, int len )
{
    strcpy( value, &static_cast< _AttrStringTemplate<std::string>* >(attr)->possible()[0] );
    return PWR_SUCCESS;
}


const char* PWR_ObjGetTypeString( PWR_ObjType type )
{
	switch( type ) {
	case PWR_OBJ_INVALID: return "Invalid";
	case PWR_OBJ_PLATFORM: return "Platform";
	case PWR_OBJ_CABINET:  return "Cabinet";
	case PWR_OBJ_BOARD:    return "Board";
	case PWR_OBJ_NODE:     return "Node";
	case PWR_OBJ_SOCKET:   return "Socket";
	case PWR_OBJ_CORE:     return "Core";
	}
}
const char* PWR_AttrGetNameString( PWR_AttrName name )
{
	switch( name ){
	case PWR_ATTR_FREQ: return "Req";
	case PWR_ATTR_POWER: return "Power";
	case PWR_ATTR_STATE: return "State";
	case PWR_ATTR_ID: return "Id";
	}	
}
