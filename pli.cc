
#include "./pli.h"
#include "./object.h"
#include "./init.h"

PLI_Context PLI_ContextInit( PLI_ContextType type )
{
    return PLI::init( type );
}

int PLI_ContextDestroy( PLI_Context ctx )
{
    return PLI::destroy( ctx );
}

PLI_Object PLI_ContextGetSelf( PLI_Context ctx )
{
    return ctx->getSelf();
}

PLI_Group PLI_ContextGetGroupByType( PLI_Context ctx, PLI_ObjectType type )
{
    return ctx->getGroup( type );
}

PLI_Group PLI_ContextCreateGroup( PLI_Context ctx, const char* name )
{
    return ctx->groupCreate( name );
}

PLI_Group PLI_ContextGetGroupByName( PLI_Context ctx, const char* name )
{
    return ctx->getGroupByName( name );
}

const char* PLI_ObjectGetName( PLI_Object obj )
{
    return &obj->name()[0];
}

const char* PLI_ObjectGetType( PLI_Object obj )
{
    return &obj->type()[0];
}

PLI_Object PLI_ObjectGetParent(PLI_Object obj )
{
    return obj->parent();
    return NULL;
}

PLI_Group PLI_ObjectGetChildren( PLI_Object obj )
{
    return& obj->children();
}

int PLI_GroupDestroy( PLI_Group group )
{
    _Context* ctx = group->getCtx();
    return ctx->groupDestroy( group );
}

int PLI_GroupAddObject( PLI_Group group, PLI_Object obj )
{
    return group->add( obj );
}

int PLI_GroupObjectRemove( PLI_Group group, PLI_Object obj )
{
    return group->remove( obj );
}

int PLI_GroupGetNumObjects( PLI_Group group )
{
    return group->size();
}

PLI_Object PLI_GroupGetObjectByIndex( PLI_Group group, int i )
{
    return group->getObject( i );
}

PLI_Object PLI_GroupGetObjectByName( PLI_Group group, int i )
{
    return NULL;
}

int PLI_ObjectGetNumAttributes( PLI_Object obj )
{
    return obj->attrGetNumber();
}


PLI_Attribute PLI_ObjectGetAttributeByIndex( PLI_Object obj, int index )
{
    return obj->attributeGet( index );
}

const char*   PLI_AttributeGetName( PLI_Attribute attr )
{
    return &attr->name()[0];
}

PLI_AttributeType PLI_AttributeGetType( PLI_Attribute attr )
{
    return attr->type();
}

int PLI_AttributeGetScale( PLI_Attribute a, PLI_AttributeScale* value )
{
    _AttributeNum* attr = dynamic_cast< _AttributeNum*>(a);
    if ( attr ) {
        return attr->unit();
    } 
    return PLI_FAILURE;
}

int PLI_AttributeFloatGetMin( PLI_Attribute attr, float* value )
{
    *value = static_cast< _AttributeNumTemplate<float>* >(attr)->min();
    return PLI_SUCCESS;
}

int PLI_AttributeFloatGetMax( PLI_Attribute attr, float* value )
{
    *value = static_cast< _AttributeNumTemplate<float>* >(attr)->max();
    return PLI_SUCCESS; 
}

int PLI_AttributeFloatGetValue( PLI_Attribute attr, float* value )
{
    *value = static_cast< _AttributeNumTemplate<float>* >(attr)->value();
    return PLI_SUCCESS;
}

int PLI_AttributeFloatSetValue( PLI_Attribute attr, float value )
{
    return static_cast< _AttributeNumTemplate<float>* >(attr)->value( value );
}

int PLI_AttributeIntGetMin( PLI_Attribute attr, int* value )
{
    *value = static_cast< _AttributeNumTemplate<int>* >(attr)->min();
    return PLI_SUCCESS;
}

int PLI_AttributeIntGetMax( PLI_Attribute attr, int* value )
{
    *value = static_cast< _AttributeNumTemplate<int>* >(attr)->max();
    return PLI_SUCCESS;
}

int PLI_AttributeIntGetValue( PLI_Attribute attr, int* value )
{
    *value = static_cast< _AttributeNumTemplate<int>* >(attr)->value();
    return PLI_SUCCESS;
}

int PLI_AttributeIntSetValue( PLI_Attribute attr, int value )
{
    static_cast< _AttributeNumTemplate<int>* >(attr)->value( value );
    return PLI_SUCCESS;
}

int PLI_AttributeStringGetValue( PLI_Attribute attr, char* value, int len )
{
    strcpy( value, &static_cast< _AttributeStringTemplate<std::string>* >(attr)->value()[0] );
    return PLI_SUCCESS; 
}

int PLI_AttributeStringGetPossible( PLI_Attribute attr, char* value, int len )
{
    strcpy( value, &static_cast< _AttributeStringTemplate<std::string>* >(attr)->possible()[0] );
    return PLI_SUCCESS;
}

