#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <pli.h> 

void  printObjectInfo( PLI_Object );
void  traverseDepth( PLI_Object );
const char * attrUnit( PLI_AttributeScale );
const char * attrType( PLI_AttributeType type );

void  printAttr( PLI_Attribute );

int main( int argc, char* argv[] )
{
    int num;
    PLI_Group group;

    // Get a context
    PLI_Context context = PLI_ContextInit( PLI_CTX_DEFAULT );; 

    // Get a group that we can add stuff to
    PLI_Group userGroup = PLI_ContextCreateGroup( context, "userGroup" );

#if 0
    traverseDepth( PLI_GetSelf( context ) );
#endif
    
    // Get all of the CORE objects
    group = PLI_ContextGetGroupByType( context, PLI_OBJ_CORE );
    assert( PLI_NULL != group );

    // Iterate over all objects in this group
    num = PLI_GroupGetNumObjects( group ); 
    int i;
    for ( i = 0; i < num; i++ ) {
        
        PLI_Object obj = PLI_GroupGetObjectByIndex( group, i );
        printf("Object `%s` type=`%s`\n", 
                PLI_ObjectGetName( obj ), PLI_ObjectGetType( obj ) );

        // how many attributes does this object have
        int numAttr = PLI_ObjectGetNumAttributes( obj );

        // iterate over the attribute
        int i;
        for ( i = 0; i < numAttr; i++ ) {
            printAttr( PLI_ObjectGetAttributeByIndex( obj, i ) );
        }

        // Add the object to another group
        PLI_GroupAddObject( userGroup, obj ); 
    }

    // Get all of the NODE objects
    group = PLI_ContextGetGroupByType( context, PLI_OBJ_NODE );
    assert( PLI_NULL != group );

    // use index based interation to loop through all objects in the group
    num = PLI_GroupGetNumObjects( group );
    for ( i = 0; i < num; i++ ) {

        // get the object at index N
        PLI_Object tmp = PLI_GroupGetObjectByIndex(group, i );

        // Add the object to another group
        PLI_GroupAddObject( userGroup, tmp ); 

        // Get the object parent(s)
        PLI_Object parent = PLI_ObjectGetParent( tmp );
        if ( PLI_NULL != parent ) {
            printf("parent %s, child%s\n", 
                    PLI_ObjectGetName(  parent ), 
                    PLI_ObjectGetName( tmp ) );
        } else {
            printf("%s\n", PLI_ObjectGetName( tmp ) ); 
        }
    } 

    PLI_Group tmpGroup = PLI_ContextGetGroupByName( context, "userGroup" );
    // print all of the objects in the user created group

    num = PLI_GroupGetNumObjects( tmpGroup );
    for ( i = 0; i < num; i++ ) {  
        printf("userGroup %s\n", PLI_ObjectGetName( 
                PLI_GroupGetObjectByIndex( tmpGroup, i ) ) );
    } 

    // cleanup
    assert( PLI_SUCCESS == PLI_GroupDestroy( userGroup ) );
    assert( PLI_SUCCESS == PLI_ContextDestroy( context ) );
    return 0;
}

void  traverseDepth( PLI_Object obj )
{
    printObjectInfo( obj );

    PLI_Group children = PLI_ObjectGetChildren( obj );

    if ( children ) {
        int num = PLI_GroupGetNumObjects( children );
        int i;
        for ( i = 0; i < num; i++ ) {
            traverseDepth( PLI_GroupGetObjectByIndex( children, i ) );
        }
    } 
}

void  printObjectInfo( PLI_Object obj ) {
    const char* name = PLI_ObjectGetName( obj );
    const char* type = PLI_ObjectGetType( obj );
    printf( "object %s is of type %s\n", name, type );
}


void  printAttr( PLI_Attribute attr )
{
    int intValue[3];
    PLI_AttributeScale scaleValue;
    float floatValue[3];
    #define STRLEN 100
    char stringValue[ STRLEN ], possible[ STRLEN ];
    PLI_AttributeType type = PLI_AttributeGetType( attr ); 

    printf("    Attr `%s` type=%s ",
             PLI_AttributeGetName( attr ), attrType(type));
    switch ( type ) {
      case PLI_ATTR_FLOAT:

        PLI_AttributeGetScale( attr, &scaleValue );
        PLI_AttributeFloatGetMin( attr, &floatValue[0]);
        PLI_AttributeFloatGetMax( attr, &floatValue[1] );
        PLI_AttributeFloatGetValue( attr, &floatValue[2] ); 

        printf("scale=%s min=%f max=%f value=%f\n", attrUnit( scaleValue ), 
                floatValue[0], floatValue[1], floatValue[2] );
        break;

      case PLI_ATTR_INT:
        PLI_AttributeGetScale( attr, &scaleValue );
        PLI_AttributeIntGetMin( attr, &intValue[0] );
        PLI_AttributeIntGetMax( attr, &intValue[1] );
        PLI_AttributeIntGetValue( attr, &intValue[2] ); 

        printf("scale=%s min=%i max=%i value=%i\n", attrUnit( scaleValue ),
            intValue[0], intValue[1], intValue[2] );
        break;

      case PLI_ATTR_STRING:
    
        PLI_AttributeStringGetPossible( attr, possible, STRLEN ),
        PLI_AttributeStringGetValue( attr, stringValue, STRLEN);
        printf("possible=`%s` value=`%s`\n", possible, stringValue );
        break;
    }
}

const char * attrType( PLI_AttributeType type )
{
    switch( type ) {
      case PLI_ATTR_FLOAT:
        return "float";
      case PLI_ATTR_INT:
        return "int";
      case PLI_ATTR_STRING:
        return "string";
    }
    return "";
}

const char * attrUnit( PLI_AttributeScale scale )
{
    switch( scale ) {
      case PLI_ATTR_UNIT_1:
        return "1";
      case PLI_ATTR_UNIT_KILO:
        return "Kilo";
      case PLI_ATTR_UNIT_MEGA:
        return "Mega";
      case PLI_ATTR_UNIT_GIGA:
        return "Giga";
      case PLI_ATTR_UNIT_TERA:
        return "Tera";
      case PLI_ATTR_UNIT_PETA:
        return "Petra";
      case PLI_ATTR_UNIT_EXA:
        return "Exa";
    }
    return "";
}

