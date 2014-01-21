
#include "./pow.h"
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

/*
* Subset of API that works on Obj
*/

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

int PWR_ObjGetNumAttrs( PWR_Obj obj )
{
    return obj->attrGetNumber();
}

int PWR_ObjGetAttrTypeByIndx( PWR_Obj obj, int index, PWR_AttrType *value )
{
    *value = obj->attributeGet( index )->name();
    return PWR_SUCCESS;
}

int PWR_ObjAttrGetValueType( PWR_Obj obj, PWR_AttrType type, 
					PWR_AttrValueType * value )
{
    _Attr* attr = obj->findAttrType( type ); 
    if ( ! attr ) {
        return PWR_FAILURE;
    }	
    *value = attr->type();
    return PWR_SUCCESS;
}

int PWR_ObjAttrGetUnits( PWR_Obj obj, PWR_AttrType type, PWR_AttrUnits* value )
{
    _AttrNum* attr = static_cast<_AttrNum*>(obj->findAttrType( type )); 
    if ( ! attr ) {
        return PWR_FAILURE;
    }	
    *value = attr->unit();
    return PWR_SUCCESS;
}

int PWR_ObjAttrFloatGetRange( PWR_Obj obj, PWR_AttrType type, float* min, float* max )
{
    _Attr*  attr = obj->findAttrType( type ); 
    if ( ! attr ) {
        return PWR_FAILURE;
    }	

    *min = static_cast< _AttrNumTemplate<float>* >(attr)->min();
    *max = static_cast< _AttrNumTemplate<float>* >(attr)->max();

    return PWR_SUCCESS;
}

int PWR_ObjAttrFloatGetValue( PWR_Obj obj, PWR_AttrType type, float* value )
{
    _Attr* attr = obj->findAttrType( type ); 
    if ( ! attr ) {
        return PWR_FAILURE;
    }	
    *value = static_cast< _AttrNumTemplate<float>* >(attr)->value();
    return PWR_SUCCESS;
}

int PWR_ObjAttrFloatSetValue( PWR_Obj obj, PWR_AttrType type, float value )
{
    _Attr* attr = obj->findAttrType( type ); 
    if ( ! attr ) {
        return PWR_FAILURE;
    }	
    return static_cast< _AttrNumTemplate<float>* >(attr)->value( value );
}

int PWR_ObjAttrIntGetRange( PWR_Obj obj, PWR_AttrType type, int* min, int* max )
{
    _Attr* attr = obj->findAttrType( type ); 
    if ( ! attr ) {
        return PWR_FAILURE;
    }	
    *min = static_cast< _AttrNumTemplate<int>* >(attr)->min();
    *max = static_cast< _AttrNumTemplate<int>* >(attr)->max();
    return PWR_SUCCESS;
}

int PWR_ObjAttrIntGetValue( PWR_Obj obj, PWR_AttrType type, int* value )
{
    _Attr* attr = obj->findAttrType( type ); 
    if ( ! attr ) {
        return PWR_FAILURE;
    }	
    *value = static_cast< _AttrNumTemplate<int>* >(attr)->value();
    return PWR_SUCCESS;
}

int PWR_ObjAttrIntSetValue( PWR_Obj obj, PWR_AttrType type, int value )
{
    _Attr* attr = obj->findAttrType( type ); 
    if ( ! attr ) {
        return PWR_FAILURE;
    }	
    static_cast< _AttrNumTemplate<int>* >(attr)->value( value );
    return PWR_SUCCESS;
}

int PWR_ObjAttrStringGetValue( PWR_Obj obj, PWR_AttrType type,
						char* value, int len )
{
    _Attr* attr = obj->findAttrType( type ); 
    if ( ! attr ) {
        return PWR_FAILURE;
    }	
    strcpy( value, &static_cast< _AttrStringTemplate<std::string>* >(attr)->value()[0] );
    return PWR_SUCCESS; 
}

int PWR_ObjAttrStringGetPossible( PWR_Obj obj, PWR_AttrType type,
						char* value, int len )
{
    _Attr* attr = obj->findAttrType( type ); 
    if ( ! attr ) {
        return PWR_FAILURE;
    }	
    strcpy( value, &static_cast< _AttrStringTemplate<std::string>* >(attr)->possible()[0] );
    return PWR_SUCCESS;
}

/*
* Subset of API that works on Grp
*/

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

const char* PWR_ObjGetTypeString( PWR_ObjType type )
{
	switch( type ) {
	case PWR_OBJ_PLATFORM: return "Platform";
	case PWR_OBJ_CABINET:  return "Cabinet";
	case PWR_OBJ_BOARD:    return "Board";
	case PWR_OBJ_NODE:     return "Node";
	case PWR_OBJ_SOCKET:   return "Socket";
	case PWR_OBJ_CORE:     return "Core";
	}
}
const char* PWR_AttrGetTypeString( PWR_AttrType name )
{
	switch( name ){
	case PWR_ATTR_NAME: return "Name";
	case PWR_ATTR_FREQ: return "Freq";
	case PWR_ATTR_PSTATE: return "Pstate";
	}	
}
