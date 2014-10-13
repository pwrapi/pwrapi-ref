#ifndef _PWR_STAT_H
#define _PWR_STAT_H

#include "pow.h"

namespace PowerAPI {

class Grp;
class ObjTreeNode;

class Stat {
  public:
	Stat( ObjTreeNode* obj, PWR_AttrName name, PWR_AttrStat stat ) 
	  : m_obj(obj), m_grp(NULL), m_attrName( name ), m_attrStat( stat ) {
 	}

	Stat( Grp* grp, PWR_AttrName name, PWR_AttrStat stat ) 
	  : m_obj(NULL), m_grp(grp), m_attrName( name ), m_attrStat( stat ) {
 	}

	int start( ) {
		return PWR_RET_FAILURE;
	}

	int stop( ) {
		return PWR_RET_FAILURE;
	}

	int clear( ) {
		return PWR_RET_FAILURE;
	}

	int getValue( double* value, PWR_StatTimes* statTimes ) {
		return PWR_RET_FAILURE;
	}

	int getValues( double value[], PWR_StatTimes statTimes[] ) {
		return PWR_RET_FAILURE;
	}

  private:
	ObjTreeNode*	m_obj;
	Grp*			m_grp;
	PWR_AttrName	m_attrName;
	PWR_AttrStat    m_attrStat;
};

}

#endif
