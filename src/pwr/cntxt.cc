
#include "cntxt.h"
#include "group.h"
#include "config.h"
#include "deviceStat.h"

using namespace PowerAPI;

Object* Cntxt::getObjByName( std::string name )
{
    DBGX("\n");
    return findObject( name );
}

Object* Cntxt::getSelf()
{
    DBGX("\n");
    return m_rootObj;
}

Object* Cntxt::getEntryPoint() {
    return getSelf();
}

Object* Cntxt::getParent( Object* obj )
{
    std::string parentName = m_config->findParent( obj->name() );
    if ( m_objMap.find(parentName) == m_objMap.end() ) {
        m_objMap[ parentName ] = findObject( parentName );
    }
    return m_objMap[ parentName ];
}

Grp* Cntxt::getChildren( Object* obj )
{
    DBGX("\n");
    Grp* grp = new Grp( this, "" );
    std::deque< std::string > children = m_config->findChildren( obj->name() );

    std::deque< std::string >::iterator iter = children.begin();

    for ( ; iter != children.end(); ++iter ) {
        grp->add( findObject( *iter ) );
    }
    return grp;
}

Grp* Cntxt::getGrp( PWR_ObjType type )
{
    Grp* grp;
    DBGX("\n");
    std::string tmp = objTypeToString( type );
    tmp = "internal" + tmp;

    if ( m_groupMap.find( tmp ) == m_groupMap.end() ) {
        grp = groupCreate(tmp);
        findAllObjType( getSelf(), type, grp );
    }

    return grp;
}

Grp* Cntxt::getGrpByName( std::string name )
{
    DBGX("\n");
    if ( m_groupMap.find( name ) == m_groupMap.end() ) {
        return NULL;
    }
    return m_groupMap[name];
}

Grp* Cntxt::groupCreate( std::string name ) {
    DBGX("\n");
    if ( m_groupMap.find( name ) != m_groupMap.end() ) {
        return NULL;
    }
    Grp* grp = new Grp( this, name );
    m_groupMap[name] = grp;
    return grp;
}

int Cntxt::groupDestroy( Grp* grp ) {
    DBGX("\n");
    int retval = PWR_RET_FAILURE;
    std::map<std::string,Grp*>::iterator iter = m_groupMap.begin();
    for ( ; iter != m_groupMap.end(); ++iter ) {
        if ( iter->second == grp ) {
            delete grp;
            m_groupMap.erase( iter );
            retval = PWR_RET_SUCCESS;
            break;
        }
    }
    return retval;
}

Object* Cntxt::createObject( std::string name, PWR_ObjType type,
													Cntxt* cntxt )
{
	DBGX("obj=`%s`\n",name.c_str() );
	return new Object( name, type, cntxt );
}

Object* Cntxt::findObject( std::string name ) {
    DBGX("obj=`%s`\n",name.c_str());
    if ( m_objMap.find( name ) == m_objMap.end() ) {
        m_objMap[name] = createObject(name, m_config->objType(name), this );
    }

    return m_objMap[name];;
}

void Cntxt::findAllObjType( Object* obj, PWR_ObjType type, Grp* grp )
{
    DBGX("%s %s %p\n", obj->name().c_str(), objTypeToString(type), grp );
    if ( obj->type() == type ) {
        DBGX("add %s %s\n", obj->name().c_str(), objTypeToString(type) );
        grp->add( obj );
    }

    std::deque< std::string > children = m_config->findChildren( obj->name() );

    std::deque< std::string >::iterator iter = children.begin();

    for ( ; iter != children.end(); ++iter ) {
        findAllObjType( findObject( *iter), type, grp );
    }
}

static double opAvg( std::vector<double>& data )
{
    double result = 0;
    for ( unsigned i =0; i < data.size(); i++ ) {
        result += data[i];
    }
    return result / data.size();
}

Stat* Cntxt::createStat( Object* obj, PWR_AttrName name, PWR_AttrStat stat )
{
    DBGX("\n");
	return new DeviceStat( this, obj, name, opAvg, 10.0 );
}

Stat* Cntxt::createStat( Grp* grp, PWR_AttrName name, PWR_AttrStat stat )
{
    DBGX("\n");
	return new DeviceStat( this, grp, name, opAvg, 10.0 );
}

int Cntxt::destroyStat( Stat* stat )
{
    DBGX("\n");
    delete stat;
    return PWR_RET_SUCCESS;
}
