
#include "./pli.h"
#include "./object.h"
#include "./init.h"

PLI_Cntxt PLI_CntxtInit( PLI_CntxtType type, const char* name )
{
    return PLI::init( type );
}

int PLI_CntxtDestroy( PLI_Cntxt ctx )
{
    return PLI::destroy( ctx );
}

PLI_Obj PLI_CntxtGetSelf( PLI_Cntxt ctx )
{
    return ctx->getSelf();
}

PLI_Grp PLI_CntxtGetGrpByType( PLI_Cntxt ctx, PLI_ObjType type )
{
    return ctx->getGrp( type );
}

PLI_Grp PLI_CntxtCreateGrp( PLI_Cntxt ctx, const char* name )
{
    return ctx->groupCreate( name );
}

PLI_Grp PLI_CntxtGetGrpByName( PLI_Cntxt ctx, const char* name )
{
    return ctx->getGrpByName( name );
}

const char* PLI_ObjGetName( PLI_Obj obj )
{
    return &obj->name()[0];
}

const char* PLI_ObjGetType( PLI_Obj obj )
{
    return &obj->type()[0];
}

PLI_Obj PLI_ObjGetParent(PLI_Obj obj )
{
    return obj->parent();
    return NULL;
}

PLI_Grp PLI_ObjGetChildren( PLI_Obj obj )
{
    return& obj->children();
}

int PLI_GrpDestroy( PLI_Grp group )
{
    _Cntxt* ctx = group->getCtx();
    return ctx->groupDestroy( group );
}

int PLI_GrpAddObj( PLI_Grp group, PLI_Obj obj )
{
    return group->add( obj );
}

int PLI_GrpObjRemove( PLI_Grp group, PLI_Obj obj )
{
    return group->remove( obj );
}

int PLI_GrpGetNumObjs( PLI_Grp group )
{
    return group->size();
}

PLI_Obj PLI_GrpGetObjByIndx( PLI_Grp group, int i )
{
    return group->getObj( i );
}

PLI_Obj PLI_GrpGetObjByName( PLI_Grp group, int i )
{
    return NULL;
}

int PLI_ObjGetNumAttrs( PLI_Obj obj )
{
    return obj->attrGetNumber();
}


PLI_Attr PLI_ObjGetAttrByIndx( PLI_Obj obj, int index )
{
    return obj->attributeGet( index );
}

const char*   PLI_AttrGetName( PLI_Attr attr )
{
    return &attr->name()[0];
}

PLI_AttrType PLI_AttrGetType( PLI_Attr attr )
{
    return attr->type();
}

int PLI_AttrGetScale( PLI_Attr a, PLI_AttrScale* value )
{
    _AttrNum* attr = dynamic_cast< _AttrNum*>(a);
    if ( attr ) {
        return attr->unit();
    } 
    return PLI_FAILURE;
}

int PLI_AttrFloatGetMin( PLI_Attr attr, float* value )
{
    *value = static_cast< _AttrNumTemplate<float>* >(attr)->min();
    return PLI_SUCCESS;
}

int PLI_AttrFloatGetMax( PLI_Attr attr, float* value )
{
    *value = static_cast< _AttrNumTemplate<float>* >(attr)->max();
    return PLI_SUCCESS; 
}

int PLI_AttrFloatGetValue( PLI_Attr attr, float* value )
{
    *value = static_cast< _AttrNumTemplate<float>* >(attr)->value();
    return PLI_SUCCESS;
}

int PLI_AttrFloatSetValue( PLI_Attr attr, float value )
{
    return static_cast< _AttrNumTemplate<float>* >(attr)->value( value );
}

int PLI_AttrIntGetMin( PLI_Attr attr, int* value )
{
    *value = static_cast< _AttrNumTemplate<int>* >(attr)->min();
    return PLI_SUCCESS;
}

int PLI_AttrIntGetMax( PLI_Attr attr, int* value )
{
    *value = static_cast< _AttrNumTemplate<int>* >(attr)->max();
    return PLI_SUCCESS;
}

int PLI_AttrIntGetValue( PLI_Attr attr, int* value )
{
    *value = static_cast< _AttrNumTemplate<int>* >(attr)->value();
    return PLI_SUCCESS;
}

int PLI_AttrIntSetValue( PLI_Attr attr, int value )
{
    static_cast< _AttrNumTemplate<int>* >(attr)->value( value );
    return PLI_SUCCESS;
}

int PLI_AttrStringGetValue( PLI_Attr attr, char* value, int len )
{
    strcpy( value, &static_cast< _AttrStringTemplate<std::string>* >(attr)->value()[0] );
    return PLI_SUCCESS; 
}

int PLI_AttrStringGetPossible( PLI_Attr attr, char* value, int len )
{
    strcpy( value, &static_cast< _AttrStringTemplate<std::string>* >(attr)->possible()[0] );
    return PLI_SUCCESS;
}

