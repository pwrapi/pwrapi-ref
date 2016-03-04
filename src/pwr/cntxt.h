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

#ifndef _CNTXT_H
#define _CNTXT_H

#include <map>
#include <pwrtypes.h>
#include <impTypes.h>
#include <string>
#include <set>

namespace PowerAPI {

class Config;
class Object;
class Group;
class Grp;
class Stat;
class AttrInfo;

class Cntxt {
  public:
	Cntxt() : m_rootObj( NULL ), m_config(NULL) {}
	virtual ~Cntxt() {}

	virtual Object* getEntryPoint();
	virtual Object* getObjByName( std::string );
	virtual Object* getParent( Object* );
	virtual Grp* 	getChildren( Object* );
	virtual Object* getSelf();
	virtual Grp*    getGrp( PWR_ObjType );
	virtual Grp* 	getGrpByName( std::string name );

	virtual Grp* 	createGrp( std::string );
	virtual int  	destroyGrp( Grp* );

	virtual Stat* 	createStat( Object*, PWR_AttrName, PWR_AttrStat );
	virtual Stat* 	createStat( Grp*, PWR_AttrName, PWR_AttrStat );
	virtual int 	destroyStat( Stat* );

	virtual AttrInfo* initAttr( Object*, PWR_AttrName ) = 0;

    virtual Object* createObject( std::string, PWR_ObjType, Cntxt* );

  protected:
    virtual Object* findObject( std::string );
	void findAllObjType( Object*, PWR_ObjType, Grp* );
    double findHz( Object* obj, PWR_AttrName name );

	Object*								m_rootObj;
	Config*         					m_config;
	std::map< std::string, Object* > 	m_objMap;
	std::map< std::string, Grp* >       m_groupMap;
};

}

#endif

