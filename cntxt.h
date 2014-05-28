#ifndef _PWR_CNTXT_h
#define _PWR_CNTXT_h

#include "tinyxml2.h"

#include "object.h"

struct _Cntxt {

  public:

    _Cntxt( PWR_CntxtType type, PWR_Role role, const char* name );

    ~_Cntxt( ) { }

    _Obj* getSelf();

    _Grp* getGrp( PWR_ObjType type )  {
        return NULL;
    }

    _Grp* getGrpByName( std::string name )  {
        return NULL;
    }

    _Grp* groupCreate( std::string name ) {
        return NULL;
    }

    int groupDestroy( _Grp* grp ) {
        return PWR_ERR_FAILURE;
    }
    _Grp* findChildren( tinyxml2::XMLElement*, _Obj* );

  private:
    tinyxml2::XMLElement* XMLFindObject( std::string );
    void printTree(  tinyxml2::XMLNode* );
    
    std::string m_configFile;
    std::string m_topName;
    _Obj*       m_top;

    tinyxml2::XMLDocument* m_xml;
};

#if 0
#if 0
    void init( _Obj* top, PWR_Role role, const char* name ) {

        m_role = role;
        m_name.insert( 0, name );
        m_top = top;
        PWR_ObjType type;

        type = PWR_OBJ_CABINET;
        m_defaultGrpMap[ type ] = new _Grp( this );
        createTypeGrp( top, type, m_defaultGrpMap[ type]  );

        type =  PWR_OBJ_BOARD;
        m_defaultGrpMap[ type ] = new _Grp( this );
        createTypeGrp( top, type, m_defaultGrpMap[ type]  );

        type =  PWR_OBJ_NODE;
        m_defaultGrpMap[ type ] = new _Grp( this );
        createTypeGrp( top, type, m_defaultGrpMap[ type]  );

        type =  PWR_OBJ_SOCKET;
        m_defaultGrpMap[ type ] = new _Grp( this );
        createTypeGrp( top, type, m_defaultGrpMap[ type]  );

        type =  PWR_OBJ_CORE;
        m_defaultGrpMap[ type ] = new _Grp( this );
        createTypeGrp( top, type, m_defaultGrpMap[ type]  );
    }
#endif


    _Grp* getGrp( PWR_ObjType type )  {

        if ( m_defaultGrpMap.find( type ) == m_defaultGrpMap.end() ) {
            return PWR_NULL;
        } else {
            return m_defaultGrpMap[type];
        }
    }

    _Grp* getGrpByName( std::string name )  {

        if ( m_groups.find( name ) == m_groups.end() ) {
            return PWR_NULL;
        } else {
            return m_groups[name];
        }
    }

    _Grp* groupCreate( std::string name ) {
        if ( m_groups.find( name ) != m_groups.end() ) {
            return NULL;
        }

        m_groups[name] = new _Grp( this );
        return m_groups[name];
    }

    int groupDestroy( _Grp* grp ) {

        std::map< std::string, _Grp* >::iterator iter = m_groups.begin();
        for ( ; iter != m_groups.end(); ++ iter ) {
            if ( iter->second == grp ) {
                delete grp;
                m_groups.erase( iter );
                return PWR_ERR_SUCCESS;
            }
        }
        return PWR_ERR_FAILURE;
    }

  private:
    void createTypeGrp( _Obj*, PWR_ObjType, _Grp* );


    std::map< PWR_ObjType, _Grp* >  m_defaultGrpMap;
    std::map< std::string, _Grp* >   m_groups;
};

inline void _Cntxt::createTypeGrp( _Obj* obj, PWR_ObjType type,
                 _Grp* group )
{
    if ( obj->type() == type ) {
        group->add( obj );
    }

    if ( ! obj->children().empty() ) {
        obj->children().reset();
        _Obj* tmp = obj->children().next();
        while ( tmp ) {
            createTypeGrp( tmp, type, group );
            tmp = obj->children().next();
        }
    }
}
#endif

#endif
