#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <pli.h> 

void  printObjInfo( PLI_Obj );
void  traverseDepth( PLI_Obj );
const char * attrUnit( PLI_AttrScale );
const char * attrType( PLI_AttrType type );

void  printAttr( PLI_Attr );

int main( int argc, char* argv[] )
{
    int num;
    PLI_Grp group;

    // Get a context
    PLI_Cntxt context = PLI_CntxtInit( PLI_CTX_DEFAULT, "App" );; 

    // Get a group that we can add stuff to
    PLI_Grp userGrp = PLI_CntxtCreateGrp( context, "userGrp" );

#if 0
    traverseDepth( PLI_GetSelf( context ) );
#endif
    
    // Get all of the CORE objects
    group = PLI_CntxtGetGrpByType( context, PLI_OBJ_CORE );
    assert( PLI_NULL != group );

    // Iterate over all objects in this group
    num = PLI_GrpGetNumObjs( group ); 
    int i;
    for ( i = 0; i < num; i++ ) {
        
        PLI_Obj obj = PLI_GrpGetObjByIndx( group, i );
        printf("Obj `%s` type=`%s`\n", 
                PLI_ObjGetName( obj ), PLI_ObjGetType( obj ) );

        // how many attributes does this object have
        int numAttr = PLI_ObjGetNumAttrs( obj );

        // iterate over the attribute
        int i;
        for ( i = 0; i < numAttr; i++ ) {
            printAttr( PLI_ObjGetAttrByIndx( obj, i ) );
        }

        // Add the object to another group
        PLI_GrpAddObj( userGrp, obj ); 
    }

    // Get all of the NODE objects
    group = PLI_CntxtGetGrpByType( context, PLI_OBJ_NODE );
    assert( PLI_NULL != group );

    // use index based interation to loop through all objects in the group
    num = PLI_GrpGetNumObjs( group );
    for ( i = 0; i < num; i++ ) {

        // get the object at index N
        PLI_Obj tmp = PLI_GrpGetObjByIndx(group, i );

        // Add the object to another group
        PLI_GrpAddObj( userGrp, tmp ); 

        // Get the object parent(s)
        PLI_Obj parent = PLI_ObjGetParent( tmp );
        if ( PLI_NULL != parent ) {
            printf("parent %s, child%s\n", 
                    PLI_ObjGetName(  parent ), 
                    PLI_ObjGetName( tmp ) );
        } else {
            printf("%s\n", PLI_ObjGetName( tmp ) ); 
        }
    } 

    PLI_Grp tmpGrp = PLI_CntxtGetGrpByName( context, "userGrp" );
    // print all of the objects in the user created group

    num = PLI_GrpGetNumObjs( tmpGrp );
    for ( i = 0; i < num; i++ ) {  
        printf("userGrp %s\n", PLI_ObjGetName( 
                PLI_GrpGetObjByIndx( tmpGrp, i ) ) );
    } 

    // cleanup
    assert( PLI_SUCCESS == PLI_GrpDestroy( userGrp ) );
    assert( PLI_SUCCESS == PLI_CntxtDestroy( context ) );
    return 0;
}

void  traverseDepth( PLI_Obj obj )
{
    printObjInfo( obj );

    PLI_Grp children = PLI_ObjGetChildren( obj );

    if ( children ) {
        int num = PLI_GrpGetNumObjs( children );
        int i;
        for ( i = 0; i < num; i++ ) {
            traverseDepth( PLI_GrpGetObjByIndx( children, i ) );
        }
    } 
}

void  printObjInfo( PLI_Obj obj ) {
    const char* name = PLI_ObjGetName( obj );
    const char* type = PLI_ObjGetType( obj );
    printf( "object %s is of type %s\n", name, type );
}


void  printAttr( PLI_Attr attr )
{
    int intValue[3];
    PLI_AttrScale scaleValue;
    float floatValue[3];
    #define STRLEN 100
    char stringValue[ STRLEN ], possible[ STRLEN ];
    PLI_AttrType type = PLI_AttrGetType( attr ); 

    printf("    Attr `%s` type=%s ",
             PLI_AttrGetName( attr ), attrType(type));
    switch ( type ) {
      case PLI_ATTR_FLOAT:

        PLI_AttrGetScale( attr, &scaleValue );
        PLI_AttrFloatGetMin( attr, &floatValue[0]);
        PLI_AttrFloatGetMax( attr, &floatValue[1] );
        PLI_AttrFloatGetValue( attr, &floatValue[2] ); 

        printf("scale=%s min=%f max=%f value=%f\n", attrUnit( scaleValue ), 
                floatValue[0], floatValue[1], floatValue[2] );
        break;

      case PLI_ATTR_INT:
        PLI_AttrGetScale( attr, &scaleValue );
        PLI_AttrIntGetMin( attr, &intValue[0] );
        PLI_AttrIntGetMax( attr, &intValue[1] );
        PLI_AttrIntGetValue( attr, &intValue[2] ); 

        printf("scale=%s min=%i max=%i value=%i\n", attrUnit( scaleValue ),
            intValue[0], intValue[1], intValue[2] );
        break;

      case PLI_ATTR_STRING:
    
        PLI_AttrStringGetPossible( attr, possible, STRLEN ),
        PLI_AttrStringGetValue( attr, stringValue, STRLEN);
        printf("possible=`%s` value=`%s`\n", possible, stringValue );
        break;
    }
}

const char * attrType( PLI_AttrType type )
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

const char * attrUnit( PLI_AttrScale scale )
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

