#include "./objectUrl.h"
#include "./objectEl.h"
#include "./object.h"

#if 0
static std::map< PWR_Obj, _Obj* > _rosebudMap;
static int objNum = 0; 
#endif

PWR_Obj rosebud( _Obj* obj ) 
{
	return obj;
#if 0
	_rosebudMap[objNum] = obj;
    return objNum++;
#endif
}
