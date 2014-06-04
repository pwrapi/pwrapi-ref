#ifndef _PWR_CNTXT_h
#define _PWR_CNTXT_h

#include "tinyxml2.h"

#include "./object.h"
#include "./dev.h"

struct _Cntxt {

  public:

    _Cntxt( PWR_CntxtType type, PWR_Role role, const char* name );

    ~_Cntxt( ) { }

    _Obj* getSelf();

    _Grp* getGrp( PWR_ObjType type ) {
        _Grp* grp = getGrpByName( objTypeToString( type ) ); 
        if ( ! grp ) {
            grp = initGrp( type ); 
        }
        return grp; 
    }

    _Grp* getGrpByName( std::string name ) {
        if ( m_groupMap.find( name ) == m_groupMap.end() ) {
            return NULL;
        }
        return m_groupMap[name];
    }

    _Grp* groupCreate( std::string name ) {
        return NULL;
    }

    int groupDestroy( _Grp* grp ) {
        return PWR_ERR_FAILURE;
    }
    _Grp* findChildren( tinyxml2::XMLElement*, _Obj* );

    _Dev* findDev( const std::string name, const std::string config );

  private:
    tinyxml2::XMLElement* XMLFindObject( std::string );
    void printTree(  tinyxml2::XMLNode* );
    void initDevices( tinyxml2::XMLElement* );
    
    _Grp* initGrp( PWR_ObjType type );
    std::string m_configFile;
    std::string m_topName;
    _Obj*       m_top;

    tinyxml2::XMLDocument* m_xml;

    std::map<std::string,_Obj*>       m_objMap;
    std::map<std::string,std::string> m_devLibMap;
    std::map<std::string,_Dev*>       m_devMap; 
    std::map<std::string,_Grp*>       m_groupMap;
};

#endif
