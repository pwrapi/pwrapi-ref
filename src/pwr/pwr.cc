/* 
 * Copyright 2014-2015 Sandia Corporation. Under the terms of Contract
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
#include "distRequest.h"
#include "distObject.h"

#include "pwr.h"
#include "group.h"
#include "status.h"
#include "object.h"
#include "stat.h"

using namespace PowerAPI;

#define CNTXT(ptr) 		 ((Cntxt*) ptr) 
#define DISTCNTXT(ptr)   ((DistCntxt*) ptr) 
#define OBJECT(ptr)      ((Object*) ptr)
#define STATUS(ptr)      ((Status*) ptr) 
#define GRP(ptr) 	     ((Grp*) ptr) 
#define STAT(ptr) 	     ((Stat*) ptr) 
#define REQUEST(ptr)     ((Request*) ptr) 

int PWR_GetMajorVersion()
{
	assert(0);
}

int PWR_GetMinorVersion()
{
	assert(0);
}

PWR_Cntxt PWR_CntxtInit( PWR_CntxtType type, PWR_Role role, const char* name )
{
	DBG( "\n" );
	return new DistCntxt( type, role, name );
}

int PWR_CntxtDestroy( PWR_Cntxt ctx )
{
	delete CNTXT(ctx);
    return PWR_RET_SUCCESS;
}

PWR_Obj PWR_CntxtGetEntryPoint( PWR_Cntxt ctx )
{
    return CNTXT(ctx)->getSelf();
}

PWR_Grp PWR_CntxtGetGrpByType( PWR_Cntxt ctx, PWR_ObjType type )
{
    return CNTXT(ctx)->getGrp( type );
}

PWR_Grp PWR_CntxtGetGrpByName( PWR_Cntxt ctx, const char* name )
{
    return CNTXT(ctx)->getGrpByName( name );
}

PWR_Obj PWR_CntxtGetObjByName( PWR_Cntxt ctx, const char* name )
{
    return CNTXT(ctx)->getObjByName( name );
}

/*
* Subset of API that works on Obj
*/

PWR_ObjType PWR_ObjGetType( PWR_Obj obj )
{
    return OBJECT(obj)->type();
}

PWR_Obj PWR_ObjGetParent(PWR_Obj obj )
{
    return OBJECT(obj)->parent();
}

PWR_Grp PWR_ObjGetChildren( PWR_Obj obj )
{
    return OBJECT(obj)->children();
}

int PWR_ObjAttrIsValid( PWR_Obj obj, PWR_AttrName type )
{
   return OBJECT(obj)->attrIsValid( type );
}

int PWR_ObjAttrGetValue( PWR_Obj obj, PWR_AttrName type, void* ptr, PWR_Time* ts )
{
	DBG("\n");
    return OBJECT(obj)->attrGetValue( type, ptr, ts );
}

int PWR_ObjAttrGetValue_NB( PWR_Obj obj, PWR_AttrName type, void* ptr, PWR_Time* ts, PWR_Request req )
{
    return OBJECT(obj)->attrGetValue( type, ptr, ts, REQUEST(req) );
}

int PWR_ObjAttrSetValue( PWR_Obj obj, PWR_AttrName type, void* ptr )
{
    return OBJECT(obj)->attrSetValue( type, ptr );
}

int PWR_ObjAttrSetValue_NB( PWR_Obj obj, PWR_AttrName attr, void* buf,
		                                                    PWR_Request req )
{
    return OBJECT(obj)->attrSetValue( attr, buf, REQUEST(req) );
}

int PWR_ObjAttrGetValues( PWR_Obj obj, int num, PWR_AttrName attrs[],
                    void* values, PWR_Time ts[], PWR_Status status )
{
    return OBJECT(obj)->attrGetValues( num, attrs, values, ts, STATUS(status) );
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
		PWR_Time* time, double period, unsigned int* count, void* buf )
{
    return OBJECT(obj)->attrGetSamples( name, time, period, count, buf );
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
		PWR_Time* time, double period, unsigned int* count, void* buf,  PWR_Request req )
{
    return OBJECT(obj)->attrGetSamples( name, time, period, count, buf, REQUEST(req) );
}


/*
* Subset of API that works on Grp
*/

PWR_Grp PWR_GrpCreate( PWR_Cntxt ctx, const char* name )
{
    return CNTXT(ctx)->groupCreate( name );
}

int PWR_GrpDestroy( PWR_Grp group )
{
    Cntxt* ctx = GRP(group)->getCntxt();
    return ctx->groupDestroy( GRP(group) );
}

const char* PWR_GrpGetName( PWR_Grp group ) 
{
    return &GRP(group)->name()[0];
}

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

PWR_Obj PWR_GrpGetObjByIndx( PWR_Grp group, int i )
{
    return GRP(group)->getObj( i );
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

int PWR_GrpAttrSetValues( PWR_Grp grp, int num, PWR_AttrName attr[], void* buf, PWR_Status status )
{
    return GRP(grp)->attrSetValues( num, attr, buf, STATUS(status) ); 
}

int PWR_GrpAttrGetValues( PWR_Grp grp, int num, PWR_AttrName attr[],
                                  void* buf, PWR_Time ts[], PWR_Status status)
{
    return GRP(grp)->attrGetValues( num, attr, buf, ts, STATUS(status) ); 
}


PWR_Status PWR_StatusCreate()
{
    return new Status;
}

int PWR_StatusDestroy(PWR_Status status )
{
    delete STATUS(status);
    return PWR_RET_SUCCESS;
}

int PWR_StatusPopError(PWR_Status status, PWR_AttrAccessError* err )
{
    return STATUS(status)->pop( err );
}

int PWR_StatusClear( PWR_Status status )
{
    return STATUS(status)->clear();
}

PWR_Stat PWR_ObjCreateStat( PWR_Obj obj, PWR_AttrName name, PWR_AttrStat stat )
{
	PWR_Stat tmp = OBJECT(obj)->getCntxt()->createStat(OBJECT(obj),name,stat);
	return tmp;
}

PWR_Stat PWR_GrpCreateStat( PWR_Grp grp, PWR_AttrName name, PWR_AttrStat stat )
{
	PWR_Stat tmp =GRP(grp)->getCntxt()->createStat(GRP(grp),name,stat);
	return tmp;
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

int PWR_StatGetValue( PWR_Stat stat, double* value, PWR_StatTimes* statTimes )
{
	return STAT(stat)->getValue( value, statTimes );
}

int PWR_StatGetValues( PWR_Stat stat, double values[],
										PWR_StatTimes statTimes[] )
{
	return STAT(stat)->getValues( values, statTimes );
}

EventChannel* PWR_CntxtGetEventChannel( PWR_Cntxt ctx )
{
    return DISTCNTXT(ctx)->getEventChannel();
}

int PWR_CntxtMakeProgress( PWR_Cntxt ctx )
{
    return DISTCNTXT(ctx)->makeProgress();
}

int PWR_ReqCheck( PWR_Request req, int* status )
{
    return static_cast<Request*>(req)->check( status );
}

int PWR_ReqWait( PWR_Request req, int* status )
{
    return static_cast<Request*>(req)->wait( status );
}

PWR_Request PWR_ReqCreate( PWR_Cntxt ctx )
{
    return new DistRequest( static_cast<Cntxt*>(ctx) );
}

PWR_Request PWR_ReqCreateCallback( PWR_Cntxt ctx, Callback callback, void* data )
{
    return new DistRequest( static_cast<Cntxt*>(ctx), callback, data );
}

int PWR_ReqDestroy( PWR_Request req )
{
    delete static_cast<Request*>(req);
	return PWR_RET_SUCCESS;
}

const char* PWR_ObjGetName( PWR_Obj obj )
{
    return &OBJECT(obj)->name()[0];
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
			assert(0);	
    }

    return PWR_RET_FAILURE;
}

int PWR_TimeConvert( PWR_Time in, time_t* out )
{
   *out = in / 1000000000;
    return PWR_RET_SUCCESS;
}
