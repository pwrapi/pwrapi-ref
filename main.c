#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <pow.h> 

void  printObjInfo( PWR_Obj );
void  traverseDepth( PWR_Obj );
const char * attrUnit( PWR_AttrUnits );
const char * attrValueType( PWR_AttrValueType type );

void  printAllAttr( PWR_Obj );

char* getObjName( PWR_Obj obj );

int main( int argc, char* argv[] )
{
    int num;
    PWR_Grp group;

    // Get a context
    PWR_Cntxt context = PWR_CntxtInit( PWR_CNTXT_DEFAULT, "App" );; 

    // Get a group that we can add stuff to
    PWR_Grp userGrp = PWR_CntxtCreateGrp( context, "userGrp" );

#if 0 
    traverseDepth( PWR_CntxtGetSelf( context ) );
#endif
    
    // Get all of the CORE objects
    group = PWR_CntxtGetGrpByType( context, PWR_OBJ_CORE );
    assert( PWR_NULL != group );

    // Iterate over all objects in this group
    num = PWR_GrpGetNumObjs( group ); 
    int i;
    for ( i = 0; i < num; i++ ) {
        
        PWR_Obj obj = PWR_GrpGetObjByIndx( group, i );
        printf("Obj `%s` type=`%s`\n", getObjName(obj), 
		        PWR_ObjGetTypeString(PWR_ObjGetType( obj ) ) );

        // how many attributes does this object have
        int numAttr = PWR_ObjGetNumAttrs( obj );

        printAllAttr( obj );
        printf("\n");

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
            char pstr[100]; 
            char cstr[100];
            strcpy( pstr, getObjName( parent ) );
            strcpy( cstr, getObjName( tmp ) );
            printf("parent %s, child %s\n", pstr, cstr ); 
                
        } else {
            printf("%s\n", getObjName( tmp ) ); 
        }
    } 

    PWR_Grp tmpGrp = PWR_CntxtGetGrpByName( context, "userGrp" );
    // print all of the objects in the user created group

    num = PWR_GrpGetNumObjs( tmpGrp );
    for ( i = 0; i < num; i++ ) {  
        printf("userGrp %s\n", getObjName(
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
    const char* name = getObjName( obj );	
    const char* type = PWR_ObjGetTypeString( PWR_ObjGetType( obj ) );
    printf( "object %s is of type %s\n", name, type );
}


static void  printObjAttr( PWR_Obj obj, PWR_AttrType type );

void  printAllAttr( PWR_Obj obj )
{
    int i, num = PWR_ObjGetNumAttrs( obj );
    for ( i = 0; i < num; i++ ) {
        PWR_AttrType type; 
        PWR_ObjGetAttrTypeByIndx( obj, i, &type  );
        printObjAttr( obj, type );
    }
}
static void  printObjAttr( PWR_Obj obj, PWR_AttrType type )
{
    int intValue[3];
    PWR_AttrUnits scaleValue;
    float floatValue[3];
    #define STRLEN 100
    char stringValue[ STRLEN ], possible[ STRLEN ];
    PWR_AttrValueType vtype;
    PWR_ObjAttrGetValueType( obj, type, &vtype ); 

    printf("    Attr `%s` vtype=%s ",
         PWR_AttrGetTypeString( type ), attrValueType(vtype));
    switch ( vtype ) {
      case PWR_ATTR_FLOAT:

        PWR_ObjAttrGetUnits( obj, type, &scaleValue );
        PWR_ObjAttrGetRange( obj, type, &floatValue[0], &floatValue[1]);
        PWR_ObjAttrGetValue( obj, type, &floatValue[2] ); 

        printf("scale=%s min=%f max=%f value=%f\n", attrUnit( scaleValue ), 
                floatValue[0], floatValue[1], floatValue[2] );
        break;

      case PWR_ATTR_INT:
        PWR_ObjAttrGetUnits( obj, type, &scaleValue  );
        PWR_ObjAttrGetRange( obj, type, &intValue[0], &intValue[1] );
        PWR_ObjAttrGetValue( obj, type, &intValue[2] ); 

        printf("scale=%s min=%i max=%i value=%i\n", attrUnit( scaleValue ),
            intValue[0], intValue[1], intValue[2] );
        break;

      case PWR_ATTR_STRING:
    
        PWR_ObjAttrGetRange( obj, type, possible, NULL ),
        PWR_ObjAttrGetValue( obj, type, stringValue);
        printf("possible=`%s` value=`%s`\n", possible, stringValue );
        break;
    }
}

char* getObjName( PWR_Obj obj )
{
    static char name[100];
    int ret = PWR_ObjAttrGetValue( obj, PWR_ATTR_NAME, name );
    assert( PWR_SUCCESS == ret ); 
    return name;
}

const char * attrValueType( PWR_AttrValueType type )
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

