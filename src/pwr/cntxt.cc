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

#include "cntxt.h"
#include "group.h"
#include "config.h"
#include "deviceStat.h"

#include <stdlib.h>

using namespace PowerAPI;

Object* Cntxt::getObjByName( std::string name )
{
    DBGX("\n");
    return findObject( name );
}

Object* Cntxt::getSelf()
{
    DBGX("root=%p\n",m_rootObj);
    return m_rootObj;
}

Object* Cntxt::getEntryPoint() {
    return getSelf();
}

Object* Cntxt::getParent( Object* obj )
{
    if ( obj == m_rootObj ) {
        return NULL;
    }

    std::string parentName = m_config->findParent( obj->name() );

    if ( parentName.empty() ) return NULL;

    if ( m_objMap.find(parentName) == m_objMap.end() ) {
        Object* tmp = findObject( parentName );
        assert( tmp );
        m_objMap[ parentName ] = tmp;
    }
    return m_objMap[ parentName ];
}

Grp* Cntxt::getChildren( Object* obj )
{
    DBGX("%s\n",obj->name().c_str());
    Grp* grp = new Grp( this, "" );
    std::deque< std::string > children = m_config->findChildren( obj->name() );

    std::deque< std::string >::iterator iter = children.begin();

    for ( ; iter != children.end(); ++iter ) {
        DBGX("%s\n", (*iter).c_str() );
        Object* tmp = findObject( *iter );
        assert( tmp );
        grp->add( tmp ); 
    }
    return grp;
}

Grp* Cntxt::getGrp( PWR_ObjType type )
{
    Grp* grp = NULL;
    DBGX("\n");
    std::string tmp = objTypeToString( type );
    tmp = "internal" + tmp;

    if ( m_groupMap.find( tmp ) == m_groupMap.end() ) {

        grp = createGrp(tmp);

        findAllObjType( getSelf(), type, grp );
        if ( 0 == grp->size() ) {
            destroyGrp(grp); 
            return NULL;
        }
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

Grp* Cntxt::createGrp( std::string name ) {
    DBGX("\n");
    if ( m_groupMap.find( name ) != m_groupMap.end() ) {
        return NULL;
    }
    Grp* grp = new Grp( this, name );
    m_groupMap[name] = grp;
    return grp;
}

int Cntxt::destroyGrp( Grp* grp ) {
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
        PWR_ObjType type = m_config->objType(name);
        DBGX("type=`%s`\n",objTypeToString(type));
        if( type == PWR_OBJ_INVALID ) {
            return NULL;
        }
        m_objMap[name] = createObject(name, type, this );
    }

    return m_objMap[name];
}

void Cntxt::findAllObjType( Object* obj, PWR_ObjType type, Grp* grp )
{
    DBGX("%s %s %p\n", obj->name().c_str(), objTypeToString(type), grp );
    if ( obj->type() == type ) {
        DBGX("add %s %s\n", obj->name().c_str(), objTypeToString(type) );
        grp->add( obj );
		return;
    }

    std::deque< std::string > children = m_config->findChildren( obj->name() );

    std::deque< std::string >::iterator iter = children.begin();

    for ( ; iter != children.end(); ++iter ) {
        Object* tmp = findObject( *iter );
        assert( tmp );
        findAllObjType( tmp, type, grp );
    }
}

static double opAvg( std::vector<double>& data, int& pos )
{
    pos = -1;
    double result = 0;
    for ( unsigned i =0; i < data.size(); i++ ) {
        DBG("%lf\n",data[i]);
        result += data[i];
    }
    return result / data.size();
}

static double opMin( std::vector<double>& data, int& pos )
{
    double result = data[0];
	
    pos = 0;
    DBG("%d %lf\n",0,data[0]);
    for ( unsigned i =1; i < data.size(); i++ ) {
        DBG("%d %lf\n",i,data[i]);
        if ( data[i] < result ) {
            pos = i;	
            result = data[i];
        }
    }
    return result;
}

static double opMax( std::vector<double>& data, int& pos )
{
    double result = data[0];
	
    pos = 0;
    DBG("%d %lf\n",0,data[0]);
    for ( unsigned i =1; i < data.size(); i++ ) {
        DBG("%d %lf\n",i,data[i]);
        if ( data[i] > result ) {
            pos = i;	
            result = data[i];
        }
    }
    return result;
}

static Stat::OpFuncPtr getOp( std::string name ) {
    if ( ! name.compare( "Avg" ) ) {
        return opAvg;
    } else if ( ! name.compare( "Min" ) ) {
        return opMin;
    } else if ( ! name.compare( "Max" ) ) {
        return opMax;
    }
    return NULL;
}

double Cntxt::findHz( Object* obj, PWR_AttrName name )
{
    std::string tmp = m_config->findAttrHz( obj->name(), name );

    DBGX("hz=%s\n",tmp.c_str());

    if ( tmp.empty() ) {
        return 0;
    }

    double hz = strtof(  tmp.c_str(), (char**)NULL );	
    if ( 0 == hz ) {
        return 0;
    }
    return hz;
}

Stat* Cntxt::createStat( Object* obj, PWR_AttrName name, PWR_AttrStat attrStat )
{
    DBGX("\n");
    Stat::OpFuncPtr op = getOp( attrStatToString( attrStat ) );
    if ( ! op ) {
        return NULL;
    }
   
    double hz = findHz( obj, name );
    if ( 0 == hz ) {
        return NULL;
    } 

    Stat* stat = NULL;
    try {
        stat = new DeviceStat( this, obj, name, op, hz );
    }
    catch(int) {
    }

    return stat; 
}


Stat* Cntxt::createStat( Grp* grp, PWR_AttrName name, PWR_AttrStat attrStat )
{
    DBGX("\n");
    Stat::OpFuncPtr op = getOp( attrStatToString( attrStat ) );
    if ( ! op ) {
        return NULL;
    }

    double hz = findHz( grp->getObj(0), name ); 
    if ( 0 == hz ) {
        return NULL;
    }

    for ( unsigned i = 1; i < grp->size(); i++ ) {
        double tmp = findHz( grp->getObj(i), name ); 
        if ( tmp != hz ) {
            return NULL;
        }
    }

    Stat* stat = NULL;
    try {
        stat = new DeviceStat( this, grp, name, op, hz );
    }
    catch(int) {
    }

    return stat; 
}

int Cntxt::destroyStat( Stat* stat )
{
    DBGX("\n");
    delete stat;
    return PWR_RET_SUCCESS;
}
