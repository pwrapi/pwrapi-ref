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

typedef void* PWR_Cntxt;
typedef void* PWR_Grp;
typedef void* PWR_Obj;
typedef void* PWR_Status;
typedef void* PWR_Stat;

#define VENDOR_MAX_STRING_LEN 1024

#define PWR_MAJOR_VERSION 1
#define PWR_MINOR_VERSION 2
#define PWR_MAX_STRING_LEN VENDOR_MAX_STRING_LEN

typedef int PWR_CntxtType;
#define PWR_CNTXT_DEFAULT 0
#define PWR_CNTXT_VENDOR  1

typedef enum {
    PWR_ROLE_APP = 0, /* Application */
    PWR_ROLE_MC,      /* Monitor and Control */
    PWR_ROLE_OS,      /* Operating System */
    PWR_ROLE_USER,    /* User */
    PWR_ROLE_RM,      /* Resource Manager */
    PWR_ROLE_ADMIN,   /* Administrator */
    PWR_ROLE_MGR,     /* HPCS Manager */
    PWR_ROLE_ACC,     /* Accounting */
    PWR_NUM_ROLES,    /* Accounting */
    /* */
    PWR_ROLE_INVALID = -1, 
    PWR_ROLE_NOT_SPECIFIED = -2 
} PWR_Role;

typedef enum {
    PWR_OBJ_PLATFORM,
    PWR_OBJ_CABINET, 
    PWR_OBJ_CHASSIS, 
    PWR_OBJ_BOARD,
    PWR_OBJ_NODE,
    PWR_OBJ_SOCKET,
    PWR_OBJ_CORE,
    PWR_OBJ_POWER_PLANE,
    PWR_OBJ_MEM,
    PWR_OBJ_NIC,
    PWR_NUM_OBJ_TYPES,
    /* */
    PWR_OBJ_INVALID = -1, 
    PWR_OBJ_NOT_SPECIFIED = -2 
} PWR_ObjType;

typedef enum {
    PWR_ATTR_PSTATE,	      /* uint64_t */
    PWR_ATTR_CSTATE,	      /* uint64_t */
    PWR_ATTR_CSTATE_LIMIT,    /* uint64_t */
    PWR_ATTR_SSTATE,	      /* uint64_t */
    PWR_ATTR_CURRENT,         /* double, amps */
    PWR_ATTR_VOLTAGE,         /* double, volts */
    PWR_ATTR_POWER,           /* double, watts */
    PWR_ATTR_POWER_LIMIT_MIN, /* double, watts */
    PWR_ATTR_POWER_LIMIT_MAX, /* double, watts */
    PWR_ATTR_FREQ,	          /* double, Hz */
    PWR_ATTR_FREQ_LIMIT_MIN,  /* double, Hz */
    PWR_ATTR_FREQ_LIMIT_MAX,  /* double, Hz */
    PWR_ATTR_ENERGY,          /* double, joules */
    PWR_ATTR_TEMP,            /* double, degrees Celsius */
    PWR_ATTR_OS_ID,           /* uint64_t */
    PWR_ATTR_THROTTLED_TIME,  /* uint64_t */
    PWR_ATTR_THROTTLED_COUNT, /* uint64_t */
    PWR_NUM_ATTR_NAMES,
    /* */
    PWR_ATTR_INVALID = -1,
    PWR_ATTR_NOT_SPECIFIED = -2
} PWR_AttrName;

typedef enum {
    PWR_ATTR_DATA_DOUBLE = 0,
    PWR_ATTR_DATA_UINT64,
    PWR_NUM_ATTR_DATA_TYPES,
    /* */
    PWR_ATTR_DATA_INVALID = -1,
    PWR_ATTR_DATA_NOT_SPECIFIED = -2
} PWR_AttrDataType;

typedef struct {
    PWR_Obj      obj;
    PWR_AttrName name;
	int			 index;
    int          error;
} PWR_AttrAccessError;

typedef enum {
       PWR_MD_NUM = 0,         /* uint64_t */
       PWR_MD_MIN,             /* either uint64_t or double, depending on attribute type */
       PWR_MD_MAX,             /* either uint64_t or double, depending on attribute type */
       PWR_MD_PRECISION,       /* uint64_t */
       PWR_MD_ACCURACY,        /* double */
       PWR_MD_UPDATE_RATE,     /* double */
       PWR_MD_SAMPLE_RATE,     /* double */
       PWR_MD_TIME_WINDOW,     /* PWR_Time */
       PWR_MD_TS_LATENCY,      /* PWR_Time */
       PWR_MD_TS_ACCURACY,     /* PWR_Time */
       PWR_MD_MAX_LEN,         /* uint64_t, max strlen of any returned metadata string. */
       PWR_MD_NAME_LEN,        /* uint64_t, max strlen of PWR_MD_NAME */
       PWR_MD_NAME,            /* char *, C-style NULL-terminated ASCII string */
       PWR_MD_DESC_LEN,        /* uint64_t, max strlen of PWR_MD_DESC */
       PWR_MD_DESC,            /* char *, C-style NULL-terminated ASCII string */
       PWR_MD_VALUE_LEN,       /* uint64_t, max strlen returned by PWR_MetaValueAtIndex */
       PWR_MD_VENDOR_INFO_LEN, /* uint64_t, max strlen of PWR_MD_VENDOR_INFO */
       PWR_MD_VENDOR_INFO,     /* char *, C-style NULL-terminated ASCII string */
       PWR_MD_MEASURE_METHOD,  /* uint64_t, 0/1 depending on real/model mesurement */
       PWR_NUM_META_NAMES,
       /* */
       PWR_MD_INVALID = -1,
       PWR_MD_NOT_SPECIFIED = -2
} PWR_MetaName;

#define PWR_RET_WARN_TRUNC 6 
#define PWR_RET_WARN_NO_GRP_BY_NAME 5
#define PWR_RET_WARN_NO_OBJ_AT_INDEX 4
#define PWR_RET_WARN_NO_CHILDREN 3
#define PWR_RET_WARN_NO_PARENT 2
#define PWR_RET_WARN_NOT_OPTIMIZED 1
#define PWR_RET_SUCCESS 0
#define PWR_RET_FAILURE -1
#define PWR_RET_NOT_IMPLEMENTED -2
#define PWR_RET_EMPTY -3
#define PWR_RET_INVALID -4
#define PWR_RET_LENGTH -5
#define PWR_RET_NO_ATTRIB -6
#define PWR_RET_NO_META -7
#define PWR_RET_READ_ONLY -8
#define PWR_RET_BAD_VALUE -9
#define PWR_RET_BAD_INDEX -10
#define PWR_RET_OP_NOT_ATTEMPTED -11
#define PWR_RET_NO_PERM -12
#define PWR_RET_OUT_OF_RANGE -13

typedef int64_t PWR_Time; 
#define PWR_TIME_UNINIT  0
#define PWR_TIME_UNKNOWN 0

typedef struct {
    PWR_Time start;
    PWR_Time stop;
    PWR_Time instant;
} PWR_TimePeriod;

typedef enum {
    PWR_ATTR_STAT_MIN = 0,
    PWR_ATTR_STAT_MAX,
    PWR_ATTR_STAT_AVG,
    PWR_ATTR_STAT_STDEV,
    PWR_ATTR_STAT_CV,
    PWR_ATTR_STAT_SUM,
    PWR_NUM_ATTR_STATS,
    /* */
    PWR_ATTR_STAT_INVALID = -1,
    PWR_ATTR_STAT_NOT_SPECIFIED = -2
} PWR_AttrStat;

typedef enum {
    PWR_ID_USER = 0,
    PWR_ID_JOB,
    PWR_ID_RUN,
    PWR_NUM_IDS,
    /* */
    PWR_ID_INVALID = -1,
    PWR_ID_NOT_SPECIFIED = -2
} PWR_ID;

typedef struct {
    uint64_t c_state_num;
    uint64_t p_state_num;
} PWR_OperState;

typedef enum {
    PWR_REGION_DEFAULT = 0,
    PWR_REGION_SERIAL,
    PWR_REGION_PARALLEL,
    PWR_REGION_COMPUTE,
    PWR_REGION_COMMUNICATE,
    PWR_REGION_IO,
    PWR_REGION_MEM_BOUND,
    PWR_NUM_REGION_HINTS,
    /* */
    PWR_REGION_INVALID = -1,
    PWR_REGION_NOT_SPECIFIED = -2
} PWR_RegionHint;

typedef enum {
    PWR_REGION_INT_HIGHEST = 0,
    PWR_REGION_INT_HIGH,
    PWR_REGION_INT_MEDIUM,
    PWR_REGION_INT_LOW,
    PWR_REGION_INT_LOWEST,
    PWR_REGION_INT_NONE,
    PWR_NUM_REGION_INTENSITIES,
    /* */
    PWR_REGION_INT_INVALID = -1,
    PWR_REGION_INT_NOT_SPECIFIED = -2
} PWR_RegionIntensity;

typedef enum {
    PWR_SLEEP_NO = 0,
    PWR_SLEEP_SHALLOW,
    PWR_SLEEP_MEDIUM,
    PWR_SLEEP_DEEP,
    PWR_SLEEP_DEEPEST,
    PWR_NUM_SLEEP_STATES,
    /* */
    PWR_SLEEP_INVALID = -1,
    PWR_SLEEP_NOT_SPECIFIED = -2
} PWR_SleepState;

typedef enum {
    PWR_PERF_FASTEST = 0,
    PWR_PERF_FAST,
    PWR_PERF_MEDIUM,
    PWR_PERF_SLOW,
    PWR_PERF_SLOWEST,
    PWR_NUM_PERF_STATES,
    /* */
    PWR_PERF_INVALID = -1,
    PWR_PERF_NOT_SPECIFIED = -2
} PWR_PerfState;

/* Extensions to the Specification */

#define PWR_NULL NULL

#define PWR_RET_IPC -14
#define PWR_RET_STATUS -15

typedef struct {
    PWR_Time    start;
    PWR_Time    stop;
    PWR_Time    instant;
} PWR_StatTimes;

typedef void* PWR_Request;

typedef int PWR_CntxtState;

typedef void (*Callback)( void* data );

#endif
