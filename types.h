
#ifndef PLI_TYPES_H
#define PLI_TYPES_H

#include <stddef.h>

#define PLI_NULL NULL

#define PLI_SUCCESS 0
#define PLI_FAILURE -1

typedef const char* PLI_ContextType;

#define PLI_CTX_DEFAULT    "Default"
#define PLI_CTX_VENDOR     "Vendor"

typedef const char* PLI_ObjectType;

#define PLI_OBJ_PLATFORM   "Platform"
#define PLI_OBJ_CABINET    "Cabinet"
#define PLI_OBJ_BOARD      "Board"
#define PLI_OBJ_NODE       "Node"
#define PLI_OBJ_SOCKET     "Socket"
#define PLI_OBJ_CORE       "Core"

#define PLI_ATTR_FREQ  "AttrFreq"
#define PLI_ATTR_POWER "AttrVolt"
#define PLI_ATTR_STATE "AttrState"
#define PLI_ATTR_ID    "AttrID"

typedef enum {
    PLI_ATTR_FLOAT,
    PLI_ATTR_INT,
    PLI_ATTR_STRING,
} PLI_AttributeType;

typedef enum {
    PLI_ATTR_UNIT_1,
    PLI_ATTR_UNIT_KILO,
    PLI_ATTR_UNIT_MEGA,
    PLI_ATTR_UNIT_GIGA,
    PLI_ATTR_UNIT_TERA,
    PLI_ATTR_UNIT_PETA,
    PLI_ATTR_UNIT_EXA
} PLI_AttributeScale;


#endif
