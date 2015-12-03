
#ifndef _CNTXT_H
#define _CNTXT_H

#include <map>
#include <pwrtypes.h>
#include <string>
#include <set>

class EventChannel;

namespace PowerAPI {

class Config;
class Object;
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
	virtual Grp* 	groupCreate( std::string name );
	virtual int  	groupDestroy( Grp* );
	virtual Stat* 	createStat( Object*, PWR_AttrName, PWR_AttrStat );
	virtual Stat* 	createStat( Grp*, PWR_AttrName, PWR_AttrStat );
	virtual int 	destroyStat( Stat* );

	virtual AttrInfo* initAttr( Object*, PWR_AttrName ) = 0;

    virtual Object* createObject( std::string, PWR_ObjType, Cntxt* );

  protected:
    virtual Object* findObject( std::string );
	void findAllObjType( Object*, PWR_ObjType, Grp* );

	Object*								m_rootObj;
	Config*         					m_config;
	std::map< std::string, Object* > 	m_objMap;
	std::map< std::string, Grp* >       m_groupMap;
};

}

#endif

