
#ifndef PLI_OBJECT_H
#define PLI_OBJECT_H

#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "./types.h"

class _Object;
class _Context;

class _Attribute {
  public:
    _Attribute( std::string name, PLI_AttributeType type ) :
        m_name( name ), 
        m_type( type )
    {}
    virtual ~_Attribute() {}
    std::string& name() { return m_name; }  
    PLI_AttributeType type() { return m_type; }

  private:
    std::string         m_name;
    PLI_AttributeType       m_type;
};

class _AttributeNum : public _Attribute {
  public:
    _AttributeNum( std::string name, PLI_AttributeType type, 
                        PLI_AttributeScale unit ) :
        _Attribute( name, type ),
        m_unit( unit )
    {}
    PLI_AttributeScale unit() { return m_unit; }
  private:
    PLI_AttributeScale   m_unit;
};

template <class T>
class _AttributeNumTemplate : public _AttributeNum {
  public:
    _AttributeNumTemplate( std::string name, PLI_AttributeType type, 
            PLI_AttributeScale unit, T min, T max, T value ) : 
        _AttributeNum( name, type, unit ),
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
class _AttributeStringTemplate : public _Attribute {
  public:
    _AttributeStringTemplate( std::string name, PLI_AttributeType type, 
                                        T possible, T value ) : 
        _Attribute( name, type ),
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
    

class _Group {
  public:
    _Group( _Context* ctx ) : m_ctx(ctx) { reset(); }

    long size() { return m_list.size(); }
    bool empty() { return m_list.empty(); }

    _Object* getObject( int i ) { return m_list[i]; }

    void reset() { pos = m_list.begin(); }

    _Object* next() {
        if ( pos == m_list.end() ) {
            return PLI_NULL;
        } else {
            _Object* tmp = *pos;
            ++pos;
            return tmp;
        }
    }

    int add( _Object* obj ) {
        m_list.push_back( obj );
        return PLI_SUCCESS; 
    }

    int remove( _Object* obj ) {
        std::vector<_Object*>::iterator iter = m_list.begin();
        for ( ; iter != m_list.end(); ++iter ) {
            if ( *iter == obj ) {
                m_list.erase( iter );
                break;
            }
        }
        return PLI_SUCCESS;
    }
    _Context* getCtx() { return m_ctx; }

  private:
    std::vector<_Object*> m_list;
    std::vector<_Object*>::iterator pos;
    _Context*   m_ctx;
};

class _Object {
    struct Attr {
        PLI_AttributeType   type; 
        std::string     name; 
    };

  public:
    _Object(_Context* ctx, std::string name, std::string type) :
        m_ctx(ctx),
        m_name(name),
        m_type(type),
        m_children(ctx)
    { }

    std::string& name() { return m_name; }
    std::string& type() { return m_type; }

    _Group& children() { return m_children; }
    _Object* parent() { return m_parent; }
    void setParent( _Object* obj ) { m_parent = obj; }

    int attrGetNumber() { return m_attrList.size(); }

    _Attribute* attributeGet( int index ) { 
        return m_attrList[index]; 
    }
    int attributeAdd( _Attribute* attr ) {
        m_attrList.push_back( attr );
        return PLI_SUCCESS;
    }

  private:
    _Context*   m_ctx;
    std::string m_name;
    std::string m_type;
    _Group m_children; 
    _Object* m_parent; 
    
    std::vector< _Attribute* > m_attrList;
};

class _Context {

  public:

    _Context( ) {}
    void init( _Object* top ) {
        m_top = top;
        const char* type;
        
        type = PLI_OBJ_CABINET;
        m_defaultGroupMap[ type ] = new _Group( this );
        createTypeGroup( top, type, m_defaultGroupMap[ type]  );

        type =  PLI_OBJ_BOARD;
        m_defaultGroupMap[ type ] = new _Group( this );
        createTypeGroup( top, type, m_defaultGroupMap[ type]  );

        type =  PLI_OBJ_NODE;
        m_defaultGroupMap[ type ] = new _Group( this );
        createTypeGroup( top, type, m_defaultGroupMap[ type]  );

        type =  PLI_OBJ_SOCKET;
        m_defaultGroupMap[ type ] = new _Group( this );
        createTypeGroup( top, type, m_defaultGroupMap[ type]  );

        type =  PLI_OBJ_CORE;
        m_defaultGroupMap[ type ] = new _Group( this );
        createTypeGroup( top, type, m_defaultGroupMap[ type]  );
    }
    ~_Context( ) { }

    _Object* getSelf() { return m_top; }

    _Group* getGroup( PLI_ObjectType type )  { 

        if ( m_defaultGroupMap.find( type ) == m_defaultGroupMap.end() ) {
            return PLI_NULL;
        } else {
            return m_defaultGroupMap[type]; 
        }
    }  

    _Group* getGroupByName( std::string name )  { 

        if ( m_groups.find( name ) == m_groups.end() ) {
            return PLI_NULL;
        } else {
            return m_groups[name]; 
        }
    }

    _Group* groupCreate( std::string name ) {
        if ( m_groups.find( name ) != m_groups.end() ) {
            return NULL;
        }

        m_groups[name] = new _Group( this );
        return m_groups[name];
    }

    int groupDestroy( _Group* grp ) {

        std::map< std::string, _Group* >::iterator iter = m_groups.begin();
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
    void createTypeGroup( _Object*, PLI_ObjectType, _Group* );
    _Object* m_top;

    std::map< PLI_ObjectType, _Group* >  m_defaultGroupMap; 
    std::map< std::string, _Group* >   m_groups;
};

inline void _Context::createTypeGroup( _Object* obj, PLI_ObjectType type,
                 _Group* group )
{
    if ( 0 == obj->type().compare( type )  ) {
        group->add( obj );
    }

    if ( ! obj->children().empty() ) {
        obj->children().reset();  
        _Object* tmp = obj->children().next(); 
        while ( tmp ) {
            createTypeGroup( tmp, type, group );
            tmp = obj->children().next(); 
        }
    }
}

#endif
