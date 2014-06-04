
#include <string.h>
#include <sys/time.h>

#include "./status.h"
#include "./pow.h"
#include "./object.h"
#include "./init.h"

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

int PWR_ObjGetNumAttrs( PWR_Obj obj )
{
    return obj->attrGetNumber();
}

int PWR_ObjGetAttrTypeByIndx( PWR_Obj obj, int index, PWR_AttrType *value )
{
    *value = obj->attributeGet( index )->type();
    return PWR_ERR_SUCCESS;
}

int PWR_ObjAttrGetValue( PWR_Obj obj, PWR_AttrType type, void* ptr,
                    size_t len, PWR_Time* ts )
{
    return obj->attrGetValue( type, ptr, len, ts );
}

int PWR_ObjAttrSetValue( PWR_Obj obj, PWR_AttrType type, void* ptr, size_t len )
{
    return obj->attrSetValue( type, ptr, len );
}


int PWR_ObjAttrGetValues( PWR_Obj obj, int num, PWR_Value values[],
                                                 PWR_Status status )
{
    int* xxx = (int*) malloc( sizeof(int) * num );

    if ( PWR_ERR_SUCCESS != obj->attrGetValues( num, values, xxx ) ) {
        for ( int i = 0; i < num; i++ ) {
            if ( PWR_ERR_SUCCESS != xxx[i] ) { 
                status->add( obj, values[i].type, xxx[i] ); 
            }
        }
    } 

    free( xxx );

    if ( !status->empty() ) {
        return PWR_ERR_FAILURE;
    } else {
        return PWR_ERR_SUCCESS;
    }
}

int PWR_ObjAttrSetValues( PWR_Obj obj, int num, PWR_Value values[],
                                                 PWR_Status status )
{
    int* xxx = (int*) malloc( sizeof(int) * num );

    if ( PWR_ERR_SUCCESS != obj->attrSetValues( num, values, xxx ) ) {
        for ( int i = 0; i < num; i++ ) {
            if ( PWR_ERR_SUCCESS != xxx[i] ) { 
                status->add( obj, values[i].type, xxx[i] ); 
            }
        }
    } 

    free( xxx );

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

int PWR_GrpAttrSetValue( PWR_Grp grp, PWR_AttrType type, void* ptr,
                                        size_t len, PWR_Status status )
{
    return grp->attrSetValue( type, ptr, len, status );
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
	switch( type ) {
	case PWR_OBJ_PLATFORM: return "Platform";
	case PWR_OBJ_CABINET:  return "Cabinet";
	case PWR_OBJ_BOARD:    return "Board";
	case PWR_OBJ_NODE:     return "Node";
	case PWR_OBJ_SOCKET:   return "Socket";
	case PWR_OBJ_CORE:     return "Core";
	case PWR_OBJ_NIC:      return "Nic";
	case PWR_OBJ_MEM:      return "Memory";
    case PWR_OBJ_INVALID:  return "Invalid";
	}
    return NULL;
}
const char* PWR_AttrGetTypeString( PWR_AttrType name )
{
	switch( name ){
	case PWR_ATTR_NAME: return "Name";
	case PWR_ATTR_FREQ: return "Freq";
	case PWR_ATTR_PSTATE: return "Pstate";
	case PWR_ATTR_MAX_POWER: return "Max Power";
	case PWR_ATTR_MIN_POWER: return "Min Power";
	case PWR_ATTR_AVG_POWER: return "Avg Power";
	case PWR_ATTR_POWER: return "Power";
	case PWR_ATTR_VOLTAGE: return "Voltage";
	case PWR_ATTR_CURRENT: return "Current";
	case PWR_ATTR_ENERGY: return "Energy";
	case PWR_ATTR_INVALID: return "Invalid";
    default:
        assert(0);
	}	
    return NULL;
}

int PWR_AppHint( PWR_Obj obj, PWR_Hint hint) {

	switch( hint ){
	case PWR_REGION_SERIAL: return PWR_ERR_SUCCESS;
	case PWR_REGION_PARALLEL: return PWR_ERR_SUCCESS;
	case PWR_REGION_COMPUTE: return PWR_ERR_SUCCESS;
	case PWR_REGION_COMMUNICATE: return PWR_ERR_SUCCESS;
	}	
    return PWR_ERR_FAILURE;
}

int PWR_TimeConvert( PWR_Time in, time_t* out )
{
   *out = in / 1000000000;
    return PWR_ERR_SUCCESS;
}
