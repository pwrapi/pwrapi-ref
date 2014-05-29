
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
    return NULL;
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
    PWR_Value value;
    PWR_Status status = PWR_StatusCreate();
    value.type = type;
    value.ptr = ptr;
    value.len = len;
    int ret = PWR_ObjAttrGetValues( obj, 1, &value, ts, status );
    if ( PWR_ERR_SUCCESS == ret ) {
        return ret;
    } else {
        PWR_AttrAccessError err;
        PWR_StatusPopError( status, &err );
        ret = err.error;
    }
    PWR_StatusDestroy( status );

    return ret;
}


static int foo( PWR_Obj obj, PWR_AttrType type, void* ptr, size_t len )
{
    _Attr* attr = obj->attrFindType( type ); 
    if ( ! attr ) {
        return PWR_ERR_FAILURE;
    }	
    return attr->getValue( ptr, len );
}

int PWR_ObjAttrGetValues( PWR_Obj obj, int num, PWR_Value values[],
                                               PWR_Time* ts, PWR_Status status )
{
    int i;
    int err;
    for ( i = 0; i < num; i++ ) {
        err = foo( obj, values[i].type, values[i].ptr, values[i].len ); 
        if ( PWR_ERR_SUCCESS != err ) { 
            status->add( obj, values[i].type, err ); 
        }
    }

    struct timeval tv;
    gettimeofday(&tv,NULL);

    if ( ts ) {
        *ts = tv.tv_sec * 1000000000;
        *ts += tv.tv_usec * 1000; 
    }

    if ( !status->empty() ) {
        return PWR_ERR_FAILURE;
    } else {
        return PWR_ERR_SUCCESS;
    }
}

int PWR_ObjAttrSetValue( PWR_Obj obj, PWR_AttrType type, void* value, size_t len )
{
    return obj->attrSetValue( type, value, len ); 
}

/*
* Subset of API that works on Grp
*/

int PWR_GrpDestroy( PWR_Grp group )
{
    _Cntxt* ctx = group->getCtx();
    return ctx->groupDestroy( group );
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

PWR_Obj PWR_GrpGetObjByName( PWR_Grp group, int i )
{
    return NULL;
}

int PWR_GrpAttrSetValue( PWR_Grp, PWR_AttrType type, void* ptr,
                                        size_t len, PWR_Status )
{
    return PWR_ERR_FAILURE;
}

PWR_Status PWR_StatusCreate()
{
    return new _Status;
}
int PWR_StatusDestroy(PWR_Status)
{
    return PWR_ERR_FAILURE;
}


int PWR_StatusPopError(PWR_Status status, PWR_AttrAccessError* err )
{
    return status->pop( err );
}

int PWR_StatusClear( PWR_Status )
{
    return PWR_ERR_FAILURE;
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
