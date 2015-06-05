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

#include "pwr.h"
#include "objTreeNode.h"
#include "group.h"
#include "status.h"
#include "cntxt.h"
#include "stat.h"

using namespace PowerAPI;

#define CNTXT(ptr) 		 ((Cntxt*) ptr) 
#define OBJTREENODE(ptr) ((ObjTreeNode*) ptr)
#define STATUS(ptr)      ((Status*) ptr) 
#define GRP(ptr) 	     ((Grp*) ptr) 
#define STAT(ptr) 	     ((Stat*) ptr) 

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
	return new Cntxt( type, role, name );
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

PWR_Obj PWR_CntxtGetObjByName( PWR_Cntxt, const char* name )
{
	assert(0);
}

/*
* Subset of API that works on Obj
*/

PWR_ObjType PWR_ObjGetType( PWR_Obj obj )
{
    return OBJTREENODE(obj)->type();
}

PWR_Obj PWR_ObjGetParent(PWR_Obj obj )
{
    return OBJTREENODE(obj)->parent();
}

PWR_Grp PWR_ObjGetChildren( PWR_Obj obj )
{
    return OBJTREENODE(obj)->children();
}

int PWR_ObjAttrIsValid( PWR_Obj obj, PWR_AttrName type )
{
   return OBJTREENODE(obj)->attrIsValid( type );
}

int PWR_ObjAttrGetValue( PWR_Obj obj, PWR_AttrName type, void* ptr, PWR_Time* ts )
{
    return OBJTREENODE(obj)->attrGetValue( type, ptr, 8, ts );
}

int PWR_ObjAttrSetValue( PWR_Obj obj, PWR_AttrName type, void* ptr )
{
    return OBJTREENODE(obj)->attrSetValue( type, ptr, 8 );
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

    if ( PWR_RET_SUCCESS != OBJTREENODE(obj)->attrGetValues(attrsV,values,tsV,statusV) ) {
        for ( int i = 0; i < num; i++ ) {
            if ( PWR_RET_SUCCESS != statusV[i] ) { 
                STATUS(status)->add( OBJTREENODE(obj), attrsV[i], statusV[i] ); 
            }
        }
    } 
    for ( int i = 0; i < num; i++ ) {
       ts[i] = tsV[i];
    }

    if ( !STATUS(status)->empty() ) {
        return PWR_RET_FAILURE;
    } else {
        return PWR_RET_SUCCESS;
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

    if ( PWR_RET_SUCCESS != OBJTREENODE(obj)->attrSetValues( attrsV, values, statusV ) ) {
        for ( int i = 0; i < num; i++ ) {
            if ( PWR_RET_SUCCESS != statusV[i] ) { 
                STATUS(status)->add( OBJTREENODE(obj), attrsV[i], statusV[i] ); 
            }
        }
    } 

    if ( !STATUS(status)->empty() ) {
        return PWR_RET_FAILURE;
    } else {
        return PWR_RET_SUCCESS;
    }
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
    Cntxt* ctx = GRP(group)->getCtx();
    return ctx->groupDestroy( GRP(group) );
}

const char* PWR_GrpGetName( PWR_Grp group ) 
{
    return &GRP(group)->name()[0];
}

int PWR_GrpAddObj( PWR_Grp group, PWR_Obj obj )
{
    return GRP(group)->add( OBJTREENODE(obj) );
}

int PWR_GrpRemoveObj( PWR_Grp group, PWR_Obj obj )
{
    return GRP(group)->remove( OBJTREENODE(obj) );
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
    return GRP(grp)->attrSetValue( type, ptr, 8, STATUS(status) );
}

int PWR_GrpAttrGetValue( PWR_Grp grp, PWR_AttrName name, void* buf,
				PWR_Time ts[], PWR_Status )
{
	assert(0);
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
	return OBJTREENODE(obj)->getCtx()->createStat(OBJTREENODE(obj),name,stat);
}

PWR_Stat PWR_GrpCreateStat( PWR_Grp grp, PWR_AttrName name, PWR_AttrStat stat )
{
	return GRP(grp)->getCtx()->createStat(GRP(grp),name,stat);
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

const char* PWR_ObjGetName( PWR_Obj obj )
{
    return &OBJTREENODE(obj)->name()[0];
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
