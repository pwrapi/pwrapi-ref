#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <pow.h> 

int main( int argc, char* argv[] )
{
    PWR_Obj     self;
    PWR_Cntxt   cntxt;
    PWR_Time    ts;
    time_t      time;
    int         retval;
    float       value;

    // Get a context
    cntxt = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_APP, "MiniMD" );
    assert( cntxt );
    self = PWR_CntxtGetSelf( cntxt );
    assert( self );
    
    printf("Profiling `%s`\n", PWR_ObjGetTypeString( PWR_ObjGetType( self ) ) ); 

    retval = PWR_ObjAttrGetValue( self, PWR_ATTR_VOLTAGE, 
                            &value, sizeof(value), &ts );
    assert( retval == PWR_ERR_INVALID );

    retval = PWR_ObjAttrGetValue( self, PWR_ATTR_POWER, 
                            &value, sizeof(value), &ts );
    assert( retval == PWR_ERR_SUCCESS );

    PWR_TimeConvert( ts, &time );
    printf("POWER=%f %s",value,ctime(&time));

    return 0;
}
