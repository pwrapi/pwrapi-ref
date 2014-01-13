
#ifndef PLI_OBJECT_H
#define PLI_OBJECT_H

#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "./types.h"

class _Obj;
class _Cntxt;

class _Attr {
  public:
    _Attr( std::string name, PLI_AttrType type ) :
        m_name( name ), 
        m_type( type )
    {}
    virtual ~_Attr() {}
    std::string& name() { return m_name; }  
    PLI_AttrType type() { return m_type; }

  private:
    std::string         m_name;
    PLI_AttrType       m_type;
};

class _AttrNum : public _Attr {
  public:
    _AttrNum( std::string name, PLI_AttrType type, 
                        PLI_AttrScale unit ) :
        _Attr( name, type ),
        m_unit( unit )
    {}
    PLI_AttrScale unit() { return m_unit; }
  private:
    PLI_AttrScale   m_unit;
};

template <class T>
class _AttrNumTemplate : public _AttrNum {
  public:
    _AttrNumTemplate( std::string name, PLI_AttrType type, 
            PLI_AttrScale unit, T min, T max, T value ) : 
        _AttrNum( name, type, unit ),
        m_min( min ),
        m_max( max ),
        m_value( value)  {}

    T min() { return m_min; }
    T max() { return m_max; }
    T value() { return m_value; }
    int value(T value ) { 
        if ( value >= m_min && value <= m_max ) {
            m_value = value; 
            return PLI_SUCCESS;
        } else {
            return PLI_FAILURE; 
        }
    }
  private:
    T m_min;
    T m_max;
    T m_value;
};
    
template <class T>
class _AttrStringTemplate : public _Attr {
  public:
    _AttrStringTemplate( std::string name, PLI_AttrType type, 
                                        T possible, T value ) : 
        _Attr( name, type ),
        m_possible( possible ),
        m_value( value )  {}

    T& possible() { return m_possible; }
    T& value() { return m_value; }
    void value(T value ) { 
        m_value = value; 
    }
  private:
    T m_possible;
    T m_value;
};
    

class _Grp {
  public:
    _Grp( _Cntxt* ctx ) : m_ctx(ctx) { reset(); }

    long size() { return m_list.size(); }
    bool empty() { return m_list.empty(); }

    _Obj* getObj( int i ) { return m_list[i]; }

    void reset() { pos = m_list.begin(); }

    _Obj* next() {
        if ( pos == m_list.end() ) {
            return PLI_NULL;
        } else {
            _Obj* tmp = *pos;
            ++pos;
            return tmp;
        }
    }

    int add( _Obj* obj ) {
        m_list.push_back( obj );
        return PLI_SUCCESS; 
    }

    int remove( _Obj* obj ) {
        std::vector<_Obj*>::iterator iter = m_list.begin();
        for ( ; iter != m_list.end(); ++iter ) {
            if ( *iter == obj ) {
                m_list.erase( iter );
                break;
            }
        }
        return PLI_SUCCESS;
    }
    _Cntxt* getCtx() { return m_ctx; }

  private:
    std::vector<_Obj*> m_list;
    std::vector<_Obj*>::iterator pos;
    _Cntxt*   m_ctx;
};

class _Obj {
    struct Attr {
        PLI_AttrType   type; 
        std::string     name; 
    };

  public:
    _Obj(_Cntxt* ctx, std::string name, std::string type) :
        m_ctx(ctx),
        m_name(name),
        m_type(type),
        m_children(ctx)
    { }

    std::string& name() { return m_name; }
    std::string& type() { return m_type; }

    _Grp& children() { return m_children; }
    _Obj* parent() { return m_parent; }
    void setParent( _Obj* obj ) { m_parent = obj; }

    int attrGetNumber() { return m_attrList.size(); }

    _Attr* attributeGet( int index ) { 
        return m_attrList[index]; 
    }
    int attributeAdd( _Attr* attr ) {
        m_attrList.push_back( attr );
        return PLI_SUCCESS;
    }

  private:
    _Cntxt*   m_ctx;
    std::string m_name;
    std::string m_type;
    _Grp m_children; 
    _Obj* m_parent; 
    
    std::vector< _Attr* > m_attrList;
};

class _Cntxt {

  public:

    _Cntxt( ) {}
    void init( _Obj* top ) {
        m_top = top;
        const char* type;
        
        type = PLI_OBJ_CABINET;
        m_defaultGrpMap[ type ] = new _Grp( this );
        createTypeGrp( top, type, m_defaultGrpMap[ type]  );

        type =  PLI_OBJ_BOARD;
        m_defaultGrpMap[ type ] = new _Grp( this );
        createTypeGrp( top, type, m_defaultGrpMap[ type]  );

        type =  PLI_OBJ_NODE;
        m_defaultGrpMap[ type ] = new _Grp( this );
        createTypeGrp( top, type, m_defaultGrpMap[ type]  );

        type =  PLI_OBJ_SOCKET;
        m_defaultGrpMap[ type ] = new _Grp( this );
        createTypeGrp( top, type, m_defaultGrpMap[ type]  );

        type =  PLI_OBJ_CORE;
        m_defaultGrpMap[ type ] = new _Grp( this );
        createTypeGrp( top, type, m_defaultGrpMap[ type]  );
    }
    ~_Cntxt( ) { }

    _Obj* getSelf() { return m_top; }

    _Grp* getGrp( PLI_ObjType type )  { 

        if ( m_defaultGrpMap.find( type ) == m_defaultGrpMap.end() ) {
            return PLI_NULL;
        } else {
            return m_defaultGrpMap[type]; 
        }
    }  

    _Grp* getGrpByName( std::string name )  { 

        if ( m_groups.find( name ) == m_groups.end() ) {
            return PLI_NULL;
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
                return PLI_SUCCESS;
            }
        } 
        return PLI_FAILURE;
    }

  private:
    void createTypeGrp( _Obj*, PLI_ObjType, _Grp* );
    _Obj* m_top;

    std::map< PLI_ObjType, _Grp* >  m_defaultGrpMap; 
    std::map< std::string, _Grp* >   m_groups;
};

inline void _Cntxt::createTypeGrp( _Obj* obj, PLI_ObjType type,
                 _Grp* group )
{
    if ( 0 == obj->type().compare( type )  ) {
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
