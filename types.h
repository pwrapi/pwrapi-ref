
#ifndef PWR_TYPES_H
#define PWR_TYPES_H

#include <stddef.h>

#define PWR_NULL NULL

typedef struct _Cntxt* PWR_Cntxt;
typedef struct _Grp*   PWR_Grp;
typedef struct _Obj*   PWR_Obj;
typedef struct _Attr*  PWR_Attr;

typedef int PWR_CntxtState;

#define PWR_SUCCESS 0
#define PWR_FAILURE -1

typedef enum {
    PWR_CNTXT_DEFAULT,
    PWR_CNTXT_VENDOR
} PWR_CntxtType;

typedef enum {
    PWR_OBJ_INVALID,
    PWR_OBJ_PLATFORM,
    PWR_OBJ_CABINET, 
    PWR_OBJ_BOARD,
    PWR_OBJ_NODE,
    PWR_OBJ_SOCKET,
    PWR_OBJ_CORE
} PWR_ObjType;

typedef enum {
    PWR_ATTR_FREQ,
    PWR_ATTR_POWER,
    PWR_ATTR_STATE,
    PWR_ATTR_ID,
} PWR_AttrName;

typedef enum {
    PWR_ATTR_FLOAT,
    PWR_ATTR_INT,
    PWR_ATTR_STRING,
} PWR_AttrType;

typedef enum {
    PWR_ATTR_UNITS_1,
    PWR_ATTR_UNITS_KILO,
    PWR_ATTR_UNITS_MEGA,
    PWR_ATTR_UNITS_GIGA,
    PWR_ATTR_UNITS_TERA,
    PWR_ATTR_UNITS_PETA,
    PWR_ATTR_UNITS_EXA
} PWR_AttrUnits;


#endif
