/* 
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>

#include "distCntxt.h"
#include "distObject.h"
#include "distRequest.h"

#include "pwr.h"
#include "group.h"
#include "status.h"
#include "object.h"
#include "stat.h"

using namespace PowerAPI;

#define CNTXT(ptr) 		 ((Cntxt*) ptr) 
#define DISTCNTXT(ptr)   ((DistCntxt*) ptr) 
#define OBJECT(ptr)      ((Object*) ptr)
#define DISTOBJECT(ptr)  ((DistObject*) ptr)
#define STATUS(ptr)      ((Status*) ptr) 
#define GRP(ptr) 	     ((Grp*) ptr) 
#define STAT(ptr) 	     ((Stat*) ptr) 
#define REQUEST(ptr)     ((Request*) ptr) 

int PWR_CntxtInit( PWR_CntxtType type, PWR_Role role, 
						const char* name, PWR_Cntxt* ctx )
{
    if( role != PWR_ROLE_INVALID && role != PWR_ROLE_NOT_SPECIFIED ) {
        try {
	        *ctx = new DistCntxt( type, role, name );
        }
        catch ( int n ) {
            return PWR_RET_FAILURE;
        }
        return PWR_RET_SUCCESS;
    } else {
        return PWR_RET_FAILURE;
    }
}

int PWR_CntxtDestroy( PWR_Cntxt ctx )
{
	delete CNTXT(ctx);
    return PWR_RET_SUCCESS;
}

int PWR_CntxtGetEntryPoint( PWR_Cntxt ctx, PWR_Obj* obj )
{
	*obj = CNTXT(ctx)->getSelf();
    return PWR_RET_SUCCESS;
}

int PWR_CntxtGetGrpByType( PWR_Cntxt ctx, PWR_ObjType type, PWR_Grp* grp )
{
    PWR_Grp tmp = CNTXT(ctx)->getGrp( type );
    if ( tmp ) {
	    *grp = tmp; 
        return PWR_RET_SUCCESS;
    }
    return PWR_RET_FAILURE;
}

int PWR_CntxtGetGrpByName( PWR_Cntxt ctx, const char* name, PWR_Grp* grp )
{
	*grp = CNTXT(ctx)->getGrpByName( name );
    return PWR_RET_SUCCESS;
}

int PWR_CntxtGetObjByName( PWR_Cntxt ctx, const char* name, PWR_Obj* obj )
{
    *obj = CNTXT(ctx)->getObjByName( name );
    if ( *obj ) {
        return PWR_RET_SUCCESS;
    } else {
        return PWR_RET_FAILURE;
    }
}

/*
* Subset of API that works on Obj
*/

int PWR_ObjGetType( PWR_Obj obj, PWR_ObjType* type )
{
    *type = OBJECT(obj)->type();
    return PWR_RET_SUCCESS;
}

int PWR_ObjGetParent(PWR_Obj obj, PWR_Obj* out )
{
    PWR_Obj tmp = OBJECT(obj)->parent();
    if ( tmp ) {
        *out = tmp;
        return PWR_RET_SUCCESS;
    }
    return PWR_RET_WARN_NO_PARENT;
}

int PWR_ObjGetChildren( PWR_Obj obj, PWR_Grp* grp )
{
    *grp = OBJECT(obj)->children();
    return PWR_RET_SUCCESS;
}

int PWR_ObjAttrIsValid( PWR_Obj obj, PWR_AttrName type )
{
   return OBJECT(obj)->attrIsValid( type ) ? PWR_RET_SUCCESS : PWR_RET_FAILURE;
}

int PWR_ObjAttrGetValue( PWR_Obj obj, PWR_AttrName type, void* ptr, PWR_Time* ts )
{
    return OBJECT(obj)->attrGetValue( type, ptr, ts );
}

int PWR_ObjAttrGetValues_NB( PWR_Obj obj, int count, PWR_AttrName type[],
			void* ptr, PWR_Time ts[], PWR_Request req )
{
    return DISTOBJECT(obj)->attrGetValues( count, type, ptr, ts, REQUEST(req) );
}

int PWR_ObjAttrSetValue( PWR_Obj obj, PWR_AttrName type, const void* ptr )
{
    return OBJECT(obj)->attrSetValue( type, (void*)ptr );
}

int PWR_ObjAttrSetValues_NB( PWR_Obj obj, int count, PWR_AttrName attr[], 
					void* buf, PWR_Request req )
{
    return DISTOBJECT(obj)->attrSetValues( count, attr, buf, REQUEST(req) );
}

int PWR_ObjAttrGetValues( PWR_Obj obj, int num, PWR_AttrName attrs[],
                    void* values, PWR_Time ts[], PWR_Status status )
{
    return OBJECT(obj)->attrGetValues( num, (PWR_AttrName*)attrs, values, ts, STATUS(status) );
}

int PWR_ObjAttrSetValues( PWR_Obj obj, int num, PWR_AttrName attrs[],
                    void* values, PWR_Status status )
{
    return OBJECT(obj)->attrSetValues( num, attrs, values, STATUS(status) );
}

int PWR_ObjAttrStartLog( PWR_Obj obj, PWR_AttrName attr )
{
    return OBJECT(obj)->attrStartLog( attr );
}

int PWR_ObjAttrStopLog( PWR_Obj obj, PWR_AttrName attr )
{
    return OBJECT(obj)->attrStopLog( attr );
}

int PWR_ObjAttrGetSamples( PWR_Obj obj, PWR_AttrName name, 
		PWR_Time* start, double period, unsigned int* count, void* buf )
{
    return OBJECT(obj)->attrGetSamples( name, start, period, count, buf );
}

int PWR_ObjAttrStartLog_NB( PWR_Obj obj, PWR_AttrName name, PWR_Request req )
{
    return OBJECT(obj)->attrStartLog( name, REQUEST(req) );
}

int PWR_ObjAttrStopLog_NB( PWR_Obj obj, PWR_AttrName name, PWR_Request req )
{
    return OBJECT(obj)->attrStopLog( name, REQUEST(req) );
}

int PWR_ObjAttrGetSamples_NB( PWR_Obj obj, PWR_AttrName name, 
		PWR_Time* start, double period, unsigned int* count, void* buf,  PWR_Request req )
{
    return OBJECT(obj)->attrGetSamples( name, start, period, count, buf, REQUEST(req) );
}


/*
* Subset of API that works on Grp
*/

int PWR_GrpCreate( PWR_Cntxt ctx, PWR_Grp* grp ) 
{
	*grp = CNTXT(ctx)->createGrp( "" );
    return PWR_RET_SUCCESS; 
}

int PWR_GrpDestroy( PWR_Grp group )
{
    Cntxt* ctx = GRP(group)->getCntxt();
    return ctx->destroyGrp( GRP(group) );
}

int PWR_GrpDuplicate( PWR_Grp, PWR_Grp* dup )
{
    return PWR_RET_NOT_IMPLEMENTED;
}

int PWR_GrpUnion( PWR_Grp, PWR_Grp, PWR_Grp* result )
{
    return PWR_RET_NOT_IMPLEMENTED;
}

int PWR_GrpIntersection( PWR_Grp, PWR_Grp, PWR_Grp* result )
{
    return PWR_RET_NOT_IMPLEMENTED;
}

int PWR_GrpDifference( PWR_Grp, PWR_Grp, PWR_Grp* result )
{
    return PWR_RET_NOT_IMPLEMENTED;
}

#if 0
const char* PWR_GrpGetName( PWR_Grp group ) 
{
    return &GRP(group)->name()[0];
}
#endif

int PWR_GrpAddObj( PWR_Grp group, PWR_Obj obj )
{
    return GRP(group)->add( OBJECT(obj) );
}

int PWR_GrpRemoveObj( PWR_Grp group, PWR_Obj obj )
{
    return GRP(group)->remove( OBJECT(obj) );
}

int PWR_GrpGetNumObjs( PWR_Grp group )
{
    return GRP(group)->size();
}

int PWR_GrpGetObjByIndx( PWR_Grp group, int i, PWR_Obj* obj ) 
{
	*obj = GRP(group)->getObj( i );
    return PWR_RET_SUCCESS; 
}

int PWR_GrpAttrSetValue( PWR_Grp grp, PWR_AttrName type, void* ptr,
                                        PWR_Status status )
{
    return GRP(grp)->attrSetValue( type, ptr, STATUS(status) );
}

int PWR_GrpAttrGetValue( PWR_Grp grp, PWR_AttrName type, void* ptr,
				PWR_Time ts[], PWR_Status status )
{
    return GRP(grp)->attrGetValue( type, ptr, ts, STATUS(status) );
}

int PWR_GrpAttrSetValues( PWR_Grp grp, int num, PWR_AttrName attr[],
								void* buf, PWR_Status status )
{
    return GRP(grp)->attrSetValues( num, attr, buf, STATUS(status) ); 
}

int PWR_GrpAttrGetValues( PWR_Grp grp, int num, PWR_AttrName attr[],
                               	void* buf, PWR_Time ts[], PWR_Status status)
{
    return GRP(grp)->attrGetValues( num, (PWR_AttrName*)attr, buf, ts, STATUS(status) ); 
}


int PWR_StatusCreate( PWR_Cntxt, PWR_Status* status )
{
	*status = new Status;
    return PWR_RET_SUCCESS;
}

int PWR_StatusDestroy( PWR_Status status )
{
    delete STATUS(status);
    return PWR_RET_SUCCESS;
}

int PWR_StatusPopError( PWR_Status status, PWR_AttrAccessError* err )
{
    return STATUS(status)->pop( err );
}

int PWR_StatusClear( PWR_Status status )
{
    return STATUS(status)->clear();
}

int PWR_ObjAttrGetMeta( PWR_Obj, PWR_AttrName, PWR_MetaName, void* val )
{
    return PWR_RET_NOT_IMPLEMENTED;
}

int PWR_ObjAttrSetMeta( PWR_Obj, PWR_AttrName, PWR_MetaName, const void* val )
{
    return PWR_RET_NOT_IMPLEMENTED;
}

int PWR_MetaValueAtIndex( PWR_Obj, PWR_AttrName, unsigned int index, void* val, char* val_str )
{
    return PWR_RET_NOT_IMPLEMENTED;
}

int PWR_ObjCreateStat( PWR_Obj obj, PWR_AttrName name, PWR_AttrStat statOp,
			 PWR_Stat* stat ) 
{
	*stat = OBJECT(obj)->getCntxt()->createStat(OBJECT(obj),name,statOp);
	return *stat ? PWR_RET_SUCCESS : PWR_RET_FAILURE;
}

int PWR_GrpCreateStat( PWR_Grp grp, PWR_AttrName name, PWR_AttrStat statOp,
			 PWR_Stat* stat )
{
	*stat = GRP(grp)->getCntxt()->createStat(GRP(grp),name,statOp);
	return *stat ? PWR_RET_SUCCESS : PWR_RET_FAILURE;
}

int PWR_StatDestroy( PWR_Stat stat )
{
	return STAT(stat)->getCtx()->destroyStat( STAT(stat) );
}

int PWR_StatStart( PWR_Stat stat )
{
	return STAT(stat)->start();
}

int PWR_StatStop( PWR_Stat stat )
{
	return STAT(stat)->stop();
}

int PWR_StatClear( PWR_Stat stat )
{
	return STAT(stat)->stop();
}

int PWR_StatGetValue( PWR_Stat stat, double* value, PWR_TimePeriod* statTimes )
{
	return STAT(stat)->getValue( value, statTimes );
}

int PWR_StatGetValues( PWR_Stat stat, double values[],
										PWR_TimePeriod statTimes[] )
{
	return STAT(stat)->getValues( values, statTimes );
}

int PWR_StatGetReduce( PWR_Stat, PWR_AttrStat, int* index, double* val, PWR_TimePeriod* )
{
	return PWR_RET_NOT_IMPLEMENTED;
}

int PWR_GetMajorVersion( )
{
    return PWR_MAJOR_VERSION;
}

int PWR_GetMinorVersion( )
{
    return PWR_MINOR_VERSION;
}

EventChannel* PWR_CntxtGetEventChannel( PWR_Cntxt ctx )
{
    return DISTCNTXT(ctx)->getEventChannel();
}

int PWR_CntxtMakeProgress( PWR_Cntxt ctx )
{
    return DISTCNTXT(ctx)->makeProgress();
}

int PWR_ReqWait( PWR_Request req )
{
    return static_cast<Request*>(req)->wait( );
}

PWR_Request PWR_ReqCreate( PWR_Cntxt ctx, PWR_Status status )
{
    return new DistRequest( static_cast<Cntxt*>(ctx), STATUS(status) );
}

PWR_Request PWR_ReqCreateCallback( PWR_Cntxt ctx, PWR_Status status,
				Callback callback, void* data )
{
    return new DistRequest( static_cast<Cntxt*>(ctx), STATUS(status), 
										callback, data );
}

int PWR_ReqDestroy( PWR_Request req )
{
    delete static_cast<Request*>(req);
	return PWR_RET_SUCCESS;
}

int PWR_ObjGetName( PWR_Obj obj, char* str, size_t len )
{
	strncpy( str, &OBJECT(obj)->name()[0], len );
	if ( OBJECT(obj)->name().size() > len - 1 ) {
		return PWR_RET_WARN_TRUNC;
	} else {
		return PWR_RET_SUCCESS;
	} 
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

int PWR_AppHint( PWR_Obj obj, PWR_RegionHint hint)
{
    switch( hint ){
        case PWR_REGION_SERIAL:
            /* online_cpus( parallel ); */
            return PWR_RET_SUCCESS;
        case PWR_REGION_PARALLEL:
            online_cpus( 1 );
            return PWR_RET_SUCCESS;
        case PWR_REGION_COMPUTE:
            return PWR_RET_SUCCESS;
        case PWR_REGION_COMMUNICATE:
            online_cpus( 2 );
            return PWR_RET_SUCCESS;
        case PWR_REGION_IO:
            return PWR_RET_SUCCESS;
        case PWR_REGION_MEM_BOUND:
            return PWR_RET_SUCCESS;
		case PWR_REGION_DEFAULT:
        default:
			assert(0);	
    }

    return PWR_RET_FAILURE;
}

int PWR_TimeConvert( PWR_Time in, time_t* out )
{
   *out = in / 1000000000;
    return PWR_RET_SUCCESS;
}
