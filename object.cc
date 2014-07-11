#ifdef USE_ULXMLRPC
#include "./objectUrl.h"
#endif

#include "./objectEl.h"
#include "./object.h"

#if 0
static std::map< _Obj*, _Obj* > _rosebudMap;
static int objNum = 0; 
#endif

_Obj* rosebud( _Obj* obj ) 
{
	return obj;
#if 0
	_rosebudMap[objNum] = obj;
    return objNum++;
#endif
}
