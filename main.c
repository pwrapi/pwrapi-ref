#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <pli.h> 

void  printObjInfo( PWR_Obj );
void  traverseDepth( PWR_Obj );
const char * attrUnit( PWR_AttrUnits );
const char * attrType( PWR_AttrType type );

void  printAttr( PWR_Attr );

int main( int argc, char* argv[] )
{
    int num;
    PWR_Grp group;

    // Get a context
    PWR_Cntxt context = PWR_CntxtInit( PWR_CNTXT_DEFAULT, "App" );; 

    // Get a group that we can add stuff to
    PWR_Grp userGrp = PWR_CntxtCreateGrp( context, "userGrp" );

#if 0
    traverseDepth( PWR_GetSelf( context ) );
#endif
    
    // Get all of the CORE objects
    group = PWR_CntxtGetGrpByType( context, PWR_OBJ_CORE );
    assert( PWR_NULL != group );

    // Iterate over all objects in this group
    num = PWR_GrpGetNumObjs( group ); 
    int i;
    for ( i = 0; i < num; i++ ) {
        
        PWR_Obj obj = PWR_GrpGetObjByIndx( group, i );
        printf("Obj `%s` type=`%s`\n", 
                PWR_ObjGetName( obj ), 
		PWR_ObjGetTypeString(PWR_ObjGetType( obj ) ) );

        // how many attributes does this object have
        int numAttr = PWR_ObjGetNumAttrs( obj );

        // iterate over the attribute
        int i;
        for ( i = 0; i < numAttr; i++ ) {
            printAttr( PWR_ObjGetAttrByIndx( obj, i ) );
        }

        // Add the object to another group
        PWR_GrpAddObj( userGrp, obj ); 
    }

    // Get all of the NODE objects
    group = PWR_CntxtGetGrpByType( context, PWR_OBJ_NODE );
    assert( PWR_NULL != group );

    // use index based interation to loop through all objects in the group
    num = PWR_GrpGetNumObjs( group );
    for ( i = 0; i < num; i++ ) {

        // get the object at index N
        PWR_Obj tmp = PWR_GrpGetObjByIndx(group, i );

        // Add the object to another group
        PWR_GrpAddObj( userGrp, tmp ); 

        // Get the object parent(s)
        PWR_Obj parent = PWR_ObjGetParent( tmp );
        if ( PWR_NULL != parent ) {
            printf("parent %s, child%s\n", 
                    PWR_ObjGetName(  parent ), 
                    PWR_ObjGetName( tmp ) );
        } else {
            printf("%s\n", PWR_ObjGetName( tmp ) ); 
        }
    } 

    PWR_Grp tmpGrp = PWR_CntxtGetGrpByName( context, "userGrp" );
    // print all of the objects in the user created group

    num = PWR_GrpGetNumObjs( tmpGrp );
    for ( i = 0; i < num; i++ ) {  
        printf("userGrp %s\n", PWR_ObjGetName( 
                PWR_GrpGetObjByIndx( tmpGrp, i ) ) );
    } 

    // cleanup
    assert( PWR_SUCCESS == PWR_GrpDestroy( userGrp ) );
    assert( PWR_SUCCESS == PWR_CntxtDestroy( context ) );
    return 0;
}

void  traverseDepth( PWR_Obj obj )
{
    printObjInfo( obj );

    PWR_Grp children = PWR_ObjGetChildren( obj );

    if ( children ) {
        int num = PWR_GrpGetNumObjs( children );
        int i;
        for ( i = 0; i < num; i++ ) {
            traverseDepth( PWR_GrpGetObjByIndx( children, i ) );
        }
    } 
}

void  printObjInfo( PWR_Obj obj ) {
    const char* name = PWR_ObjGetName( obj );
    const char* type = PWR_ObjGetTypeString( PWR_ObjGetType( obj ) );
    printf( "object %s is of type %s\n", name, type );
}


void  printAttr( PWR_Attr attr )
{
    int intValue[3];
    PWR_AttrUnits scaleValue;
    float floatValue[3];
    #define STRLEN 100
    char stringValue[ STRLEN ], possible[ STRLEN ];
    PWR_AttrType type = PWR_AttrGetType( attr ); 

    printf("    Attr `%s` type=%s ",
             PWR_AttrGetNameString( PWR_AttrGetName( attr )), attrType(type));
    switch ( type ) {
      case PWR_ATTR_FLOAT:

        PWR_AttrGetUnits( attr, &scaleValue );
        PWR_AttrFloatGetMin( attr, &floatValue[0]);
        PWR_AttrFloatGetMax( attr, &floatValue[1] );
        PWR_AttrFloatGetValue( attr, &floatValue[2] ); 

        printf("scale=%s min=%f max=%f value=%f\n", attrUnit( scaleValue ), 
                floatValue[0], floatValue[1], floatValue[2] );
        break;

      case PWR_ATTR_INT:
        PWR_AttrGetUnits( attr, &scaleValue );
        PWR_AttrIntGetMin( attr, &intValue[0] );
        PWR_AttrIntGetMax( attr, &intValue[1] );
        PWR_AttrIntGetValue( attr, &intValue[2] ); 

        printf("scale=%s min=%i max=%i value=%i\n", attrUnit( scaleValue ),
            intValue[0], intValue[1], intValue[2] );
        break;

      case PWR_ATTR_STRING:
    
        PWR_AttrStringGetPossible( attr, possible, STRLEN ),
        PWR_AttrStringGetValue( attr, stringValue, STRLEN);
        printf("possible=`%s` value=`%s`\n", possible, stringValue );
        break;
    }
}

const char * attrType( PWR_AttrType type )
{
    switch( type ) {
      case PWR_ATTR_FLOAT:
        return "float";
      case PWR_ATTR_INT:
        return "int";
      case PWR_ATTR_STRING:
        return "string";
    }
    return "";
}

const char * attrUnit( PWR_AttrUnits scale )
{
    switch( scale ) {
      case PWR_ATTR_UNITS_1:
        return "1";
      case PWR_ATTR_UNITS_KILO:
        return "Kilo";
      case PWR_ATTR_UNITS_MEGA:
        return "Mega";
      case PWR_ATTR_UNITS_GIGA:
        return "Giga";
      case PWR_ATTR_UNITS_TERA:
        return "Tera";
      case PWR_ATTR_UNITS_PETA:
        return "Petra";
      case PWR_ATTR_UNITS_EXA:
        return "Exa";
    }
    return "";
}

