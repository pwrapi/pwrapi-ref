#ifdef USE_ULXMLRPC
#include "objectUrl.h"
#endif

#include "objTreeNode.h"

#if 0
static std::map< _Obj*, _Obj* > _rosebudMap;
static int objNum = 0; 
#endif

TreeNode* rosebud( TreeNode* obj ) 
{
	return obj;
#if 0
	_rosebudMap[objNum] = obj;
    return objNum++;
#endif
}
