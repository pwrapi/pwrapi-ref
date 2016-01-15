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

#ifndef _PWR_TYPES_H
#define _PWR_TYPES_H

#include <stddef.h>
#include <stdint.h>

#define PWR_NULL NULL

typedef void* PWR_Cntxt;
typedef void* PWR_Grp;
typedef void* PWR_Obj;
typedef void* PWR_Status;
typedef void* PWR_Stat;
typedef void* PWR_Request;

typedef int PWR_CntxtState;

typedef void (*Callback)(void* data, int status);

#define PWR_RET_WARN_DEPRECATED 1
#define PWR_RET_SUCCESS 0
#define PWR_RET_FAILURE -1
#define PWR_RET_NOT_IMPLEMENTED -2
#define PWR_RET_EMPTY -3
#define PWR_RET_INVALID -4
#define PWR_RET_LENGTH -5
#define PWR_RET_OP_NOT_ATTEMPTED -6
#define PWR_RET_NO_PERM -7

typedef int64_t PWR_Time; 
#define PWR_TIME_NOT_SET 0

typedef enum {
    PWR_CNTXT_DEFAULT,	/* Required */
    PWR_CNTXT_VENDOR    /* Optional */
} PWR_CntxtType;

typedef enum {
    PWR_ROLE_APP,   /* Application */
    PWR_ROLE_MC,    /* Monitor and Control */
    PWR_ROLE_OS,    /* Operating System */
    PWR_ROLE_USER,  /* User */
    PWR_ROLE_RM,    /* Resource Manager */
    PWR_ROLE_ADMIN, /* Administrator */
    PWR_ROLE_MGR,   /* HPCS Manager */
    PWR_ROLE_ACC    /* Accounting */
} PWR_Role;

typedef enum {
    PWR_OBJ_INVALID = 0, 
    PWR_OBJ_PLATFORM,
	PWR_OBJ_START = PWR_OBJ_PLATFORM, 
    PWR_OBJ_CABINET, 
    PWR_OBJ_BOARD,
    PWR_OBJ_NODE,
    PWR_OBJ_SOCKET,
    PWR_OBJ_CORE,
    /* */
    PWR_OBJ_MEM,
    PWR_OBJ_NIC,
	PWR_OBJ_STOP = PWR_OBJ_NIC, 
} PWR_ObjType;

#define  TOTAL_NUM_PWR_OBJS  ( ( PWR_OBJ_STOP - PWR_OBJ_START ) + 1 )

typedef enum {
    PWR_ATTR_INVALID = 0,   /* Required String */
    PWR_ATTR_NAME,	        /* Required String */
	PWR_ATTR_START = PWR_ATTR_NAME,
    PWR_ATTR_FREQ,	        /* Required Float  */
    PWR_ATTR_MAX_PCAP,      /* Required Float  */
    PWR_ATTR_MIN_PCAP,      /* Required Float  */
    PWR_ATTR_MAX_POWER,     /* Required Float  */
    PWR_ATTR_MIN_POWER,     /* Required Float  */
    PWR_ATTR_AVG_POWER,     /* Required Float  */
    PWR_ATTR_POWER,         /* Required Float  */
    PWR_ATTR_VOLTAGE,       /* Required Float  */
    PWR_ATTR_CURRENT,       /* Required Float  */
    PWR_ATTR_ENERGY,        /* Required Float  */
    PWR_ATTR_TEMP,          /* Required Float  */
    PWR_ATTR_PSTATE,	    /* Required Int    */
    PWR_ATTR_CSTATE,	    /* Required Int    */
    PWR_ATTR_SSTATE,	  	/* Required Int    */
    PWR_ATTR_FREQ_LIMIT_MAX,	/* double, Hz    */
    PWR_ATTR_FREQ_LIMIT_MIN,	/* double, Hz    */
    PWR_ATTR_NOT_SPECIFIED,
    PWR_ATTR_STOP = PWR_ATTR_NOT_SPECIFIED
} PWR_AttrName;

#define TOTAL_NUM_PWR_ATTRS ( ( PWR_ATTR_STOP - PWR_ATTR_START ) + 1 )

typedef enum {
    PWR_ATTR_STAT_MIN,
    PWR_ATTR_STAT_MAX,
    PWR_ATTR_STAT_AVG,
    PWR_ATTR_STAT_STDEV,
    PWR_ATTR_STAT_MEAN,
    PWR_ATTR_STAT_CV,
    PWR_ATTR_STAT_ETC,
} PWR_AttrStat;

typedef struct {
    PWR_Time    start;
    PWR_Time    stop;
    PWR_Time    instant;
} PWR_StatTimes;

typedef enum {
    PWR_ATTR_DATA_FLOAT,
    PWR_ATTR_DATA_INT,
    PWR_ATTR_DATA_INVALID
} PWR_AttrDataType;

typedef struct {
    PWR_Obj      obj;
    PWR_AttrName name;
    int          error;
} PWR_AttrAccessError;

typedef enum {
    PWR_REGION_DEFAULT,
    PWR_REGION_SERIAL,
    PWR_REGION_PARALLEL,
    PWR_REGION_COMPUTE,
    PWR_REGION_COMMUNICATE,
    PWR_REGION_IO,
    PWR_REGION_MEM_BOUND
} PWR_RegionHint;

typedef enum {
	PWR_REGION_INT_HIGHEST,
	PWR_REGION_INT_HIGH,
	PWR_REGION_INT_MEDIUM,
	PWR_REGION_INT_LOW,
	PWR_REGION_INT_LOWEST,
	PWR_REGION_INT_HONE
} PWR_RegionIntensity;

typedef enum {
    PWR_SLEEP_NO,
    PWR_SLEEP_SHALLOW,
    PWR_SLEEP_MEDIUM,
    PWR_SLEEP_DEEP,
    PWR_SLEEP_DEEPEST
} PWR_SleepState;

typedef enum {
    PWR_PERF_FASTEST,
    PWR_PERF_FAST,
    PWR_PERF_MEDIUM,
    PWR_PERF_SLOW,
    PWR_PERF_SLOWEST
} PWR_PerfState;

#endif
