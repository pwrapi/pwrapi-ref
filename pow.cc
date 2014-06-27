#include <string.h>
#include <sys/time.h>

#include "object.h"
#include "group.h"

#include "status.h"
#include "pow.h"
#include "cntxt.h"
#include "init.h"

PWR_Cntxt PWR_CntxtInit( PWR_CntxtType type, PWR_Role role, const char* name )
{
    return PWR::init( type, role, name );
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
}

PWR_Grp PWR_ObjGetChildren( PWR_Obj obj )
{
    return obj->children();
}

int PWR_ObjAttrIsValid( PWR_Obj obj, PWR_AttrName type )
{
   return obj->attrIsValid( type );
}

int PWR_ObjAttrGetValue( PWR_Obj obj, PWR_AttrName type, void* ptr, PWR_Time* ts )
{
    return obj->attrGetValue( type, ptr, 8, ts );
}

int PWR_ObjAttrSetValue( PWR_Obj obj, PWR_AttrName type, void* ptr )
{
    return obj->attrSetValue( type, ptr, 8 );
}

int PWR_ObjAttrGetValues( PWR_Obj obj, int num, PWR_AttrName attrs[],
                    void* values, PWR_Time ts[], PWR_Status status )
{
    std::vector<PWR_AttrName> attrsV(num);
    std::vector<PWR_Time>     tsV(num);
    std::vector<int>          statusV(num);
    for ( int i = 0; i < num; i++ ) {
        attrsV[i] =  attrs[i];
    } 

    if ( PWR_ERR_SUCCESS != obj->attrGetValues(attrsV,values,tsV,statusV) ) {
        for ( int i = 0; i < num; i++ ) {
            if ( PWR_ERR_SUCCESS != statusV[i] ) { 
                status->add( obj, attrsV[i], statusV[i] ); 
            }
        }
    } 
    for ( int i = 0; i < num; i++ ) {
       ts[i] = tsV[i];
    }

    if ( !status->empty() ) {
        return PWR_ERR_FAILURE;
    } else {
        return PWR_ERR_SUCCESS;
    }
}

int PWR_ObjAttrSetValues( PWR_Obj obj, int num, PWR_AttrName attrs[],
                    void* values, PWR_Status status )
{
    std::vector<PWR_AttrName> attrsV(num);
    std::vector<int>          statusV(num);
    for ( int i = 0; i < num; i++ ) {
        attrsV[i] =  attrs[i];
    } 

    if ( PWR_ERR_SUCCESS != obj->attrSetValues( attrsV, values, statusV ) ) {
        for ( int i = 0; i < num; i++ ) {
            if ( PWR_ERR_SUCCESS != statusV[i] ) { 
                status->add( obj, attrsV[i], statusV[i] ); 
            }
        }
    } 

    if ( !status->empty() ) {
        return PWR_ERR_FAILURE;
    } else {
        return PWR_ERR_SUCCESS;
    }
}

/*
* Subset of API that works on Grp
*/

int PWR_GrpDestroy( PWR_Grp group )
{
    _Cntxt* ctx = group->getCtx();
    return ctx->groupDestroy( group );
}

const char* PWR_GrpGetName( PWR_Grp group ) 
{
    return &group->name()[0];
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

int PWR_GrpAttrSetValue( PWR_Grp grp, PWR_AttrName type, void* ptr,
                                        PWR_Status status )
{
    return grp->attrSetValue( type, ptr, 8, status );
}

int PWR_GrpAttrSetValues( PWR_Grp grp, int num, PWR_AttrName attr[], void* buf, PWR_Status status )
{
    return grp->attrSetValues( num, attr, buf, status ); 
}

int PWR_GrpAttrGetValues( PWR_Grp grp, int num, PWR_AttrName attr[], void* buf,
                                                        PWR_Time ts[], PWR_Status status)
{
    return grp->attrGetValues( num, attr, buf, ts, status ); 
}


PWR_Status PWR_StatusCreate()
{
    return new _Status;
}
int PWR_StatusDestroy(PWR_Status status )
{
    delete status;
    return PWR_ERR_SUCCESS;
}

int PWR_StatusPopError(PWR_Status status, PWR_AttrAccessError* err )
{
    return status->pop( err );
}

int PWR_StatusClear( PWR_Status status )
{
    return status->clear();
}

const char* PWR_ObjGetName( PWR_Obj obj )
{
    return &obj->name()[0];
}

const char* PWR_ObjGetTypeString( PWR_ObjType type )
{
    return objTypeToString( type );
}
const char* PWR_AttrGetTypeString( PWR_AttrName name )
{
    return attrNameToString( name );
}

static int online_cpus(int number_desired)
{
    /* TBD - need to hook in plugin call for oshw_cpudev */        

    return 0;
}

int PWR_AppHint( PWR_Obj obj, PWR_Hint hint)
{
    switch( hint ){
        case PWR_REGION_SERIAL:
            /* online_cpus( parallel ); */
            return PWR_ERR_SUCCESS;
        case PWR_REGION_PARALLEL:
            online_cpus( 1 );
            return PWR_ERR_SUCCESS;
        case PWR_REGION_COMPUTE:
            return PWR_ERR_SUCCESS;
        case PWR_REGION_COMMUNICATE:
            online_cpus( 2 );
            return PWR_ERR_SUCCESS;
        case PWR_REGION_IO:
            return PWR_ERR_SUCCESS;
        case PWR_REGION_MEM_BOUND:
            return PWR_ERR_SUCCESS;
    }

    return PWR_ERR_FAILURE;
}

int PWR_TimeConvert( PWR_Time in, time_t* out )
{
   *out = in / 1000000000;
    return PWR_ERR_SUCCESS;
}
