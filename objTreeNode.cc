#include "objTreeNode.h"
#include "group.h"
#include "debug.h"
#include "types.h"

using namespace PowerAPI;

int ObjTreeNode::getStat( PWR_AttrName name, PWR_AttrStat stat,
                double* value, PWR_StatTimes* statTimes )
{
	DBGX("%s\n",objTypeToString(m_type));	
    assert(!m_children);
    return TreeNode::getStat( name, stat, value, statTimes );
}
int ObjTreeNode::getStats( PWR_AttrName name, PWR_AttrStat stat,
                double* value, PWR_StatTimes* statTimes )
{
	DBGX("%s\n",objTypeToString(m_type));	
    assert(!m_children);
    return TreeNode::getStats( name, stat, value, statTimes );
}
