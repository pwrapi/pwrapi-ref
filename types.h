
#ifndef PLI_TYPES_H
#define PLI_TYPES_H

#include <stddef.h>

#define PLI_NULL NULL

typedef struct _Cntxt* PLI_Cntxt;
typedef struct _Grp*   PLI_Grp;
typedef struct _Obj*   PLI_Obj;
typedef struct _Attr*  PLI_Attr;
typedef int PLI_CntxtState;

#define PLI_SUCCESS 0
#define PLI_FAILURE -1

typedef const char* PLI_CntxtType;

//typedef enum {
//    PLI_CNTXT_DEFAULT,
//    PLI_CNTXT_VENDOR
//} PLI_CntxtType;

#define PLI_CTX_DEFAULT    "Default"
#define PLI_CTX_VENDOR     "Vendor"

typedef const char* PLI_ObjType;

//typedef enum {
//    PLI_OBJ_PLATFORM,
//    PLI_OBJ_CABINET, 
//    PLI_OBJ_BOARD,
//    PLI_OBJ_NODE,
//    PLI_OBJ_SOCKET,
//    PLI_OBJ_CORE
//} PLI_ObjType;

#define PLI_OBJ_PLATFORM   "Platform"
#define PLI_OBJ_CABINET    "Cabinet"
#define PLI_OBJ_BOARD      "Board"
#define PLI_OBJ_NODE       "Node"
#define PLI_OBJ_SOCKET     "Socket"
#define PLI_OBJ_CORE       "Core"

//typedef enum {
//    PLI_ATTR_FREQ,
//    PLI_ATTR_POWER,
//    PLI_ATTR_STATE,
//    PLI_ATTR_ID,
//} PLI_ObjAttr;;

#define PLI_ATTR_FREQ  "AttrFreq"
#define PLI_ATTR_POWER "AttrVolt"
#define PLI_ATTR_STATE "AttrState"
#define PLI_ATTR_ID    "AttrID"

typedef enum {
    PLI_ATTR_FLOAT,
    PLI_ATTR_INT,
    PLI_ATTR_STRING,
} PLI_AttrType;

typedef enum {
    PLI_ATTR_UNIT_1,
    PLI_ATTR_UNIT_KILO,
    PLI_ATTR_UNIT_MEGA,
    PLI_ATTR_UNIT_GIGA,
    PLI_ATTR_UNIT_TERA,
    PLI_ATTR_UNIT_PETA,
    PLI_ATTR_UNIT_EXA
} PLI_AttrScale;


#endif
