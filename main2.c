#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <pow.h> 

int main( int argc, char* argv[] )
{
    PWR_Status status;
    PWR_Obj self;
    PWR_Cntxt cntxt;
    time_t time;
    int retval;
    float value;
    PWR_Time ts;
    PWR_Value v2;

    // Get a context
    cntxt = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "App" );
    assert( cntxt );
    self = PWR_CntxtGetSelf( cntxt );
    assert( self );
    
    printf("I'm a `%s`\n", PWR_ObjGetTypeString( PWR_ObjGetType( self ) ) ); 

    PWR_Obj parent = PWR_ObjGetParent( self );
    assert( ! parent );

    PWR_Grp children = PWR_ObjGetChildren( self );
    assert( children );

    int i;
    for ( i = 0; i < PWR_GrpGetNumObjs(children); i++ ) {
        printf("child %s\n", PWR_ObjGetName( 
                        PWR_GrpGetObjByIndx( children, i ) ) );
    }

    retval = PWR_ObjAttrGetValue( self, PWR_ATTR_VOLTAGE, 
                            &value, sizeof(value), &ts );
    assert( retval == PWR_ERR_INVALID );

    retval = PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, 
                            &value, sizeof(value), &ts );
    assert( retval == PWR_ERR_SUCCESS );

    PWR_TimeConvert( ts, &time );
    printf("POWER=%f %s\n",value,ctime(&time));

    value = 25.812;
    retval = PWR_ObjAttrSetValue( self, PWR_ATTR_POWER, 
                            &value, sizeof(value) );
    assert( retval == PWR_ERR_SUCCESS );
    printf("set value to %f\n",value);

    v2.type = PWR_ATTR_POWER;
    v2.ptr = &value;
    v2.len = sizeof(value);
    status = PWR_StatusCreate();

    retval = PWR_ObjAttrGetValues( self, 1, &v2, status );  
    assert( retval == PWR_ERR_SUCCESS );

    PWR_TimeConvert( v2.timeStamp, &time );
    printf("POWER=%f %s\n", value, ctime( &time ) );

    value = 100.10;
    retval = PWR_ObjAttrSetValues( self, 1, &v2, status );  
    assert( retval == PWR_ERR_SUCCESS );

    retval = PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, 
                            &value, sizeof(value), &ts );
    assert( retval == PWR_ERR_SUCCESS );

    PWR_TimeConvert( ts, &time );
    printf("POWER=%f %s\n",value,ctime(&time));

    return 0;
}
