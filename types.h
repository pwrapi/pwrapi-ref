
#ifndef PWR_TYPES_H
#define PWR_TYPES_H

#include <stddef.h>

#define PWR_NULL NULL

typedef struct _Cntxt* PWR_Cntxt;
typedef struct _Grp*   PWR_Grp;
typedef struct _Obj*   PWR_Obj;

typedef int PWR_CntxtState;

#define PWR_SUCCESS 0
#define PWR_FAILURE -1

typedef enum {
    PWR_CNTXT_DEFAULT,	/* Required */
    PWR_CNTXT_VENDOR    /* Optional */
} PWR_CntxtType;

typedef enum {
    PWR_OBJ_PLATFORM,
    PWR_OBJ_CABINET, 
    PWR_OBJ_BOARD,
    PWR_OBJ_NODE,
    PWR_OBJ_SOCKET,
    PWR_OBJ_CORE
} PWR_ObjType;

typedef enum {
    PWR_ATTR_NAME,	/* Required */
    PWR_ATTR_FREQ,	/* Optional */
    PWR_ATTR_PSTATE	/* Optional */
} PWR_AttrType;

typedef enum {
    PWR_ATTR_FLOAT,
    PWR_ATTR_INT,
    PWR_ATTR_STRING,
} PWR_AttrValueType;

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
