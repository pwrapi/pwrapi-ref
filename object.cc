
#include "./object.h"
#include "./util.h"

#include "./debug.h"

using namespace tinyxml2;

_Obj::_Obj( _Cntxt* ctx, _Obj* parent, tinyxml2::XMLElement* el ) :
    m_ctx(ctx),
    m_parent(parent),
    m_children( NULL ),
    m_xmlElement(el)
{
    m_type = objTypeStrToInt( m_xmlElement->Attribute("type") );

    assert( PWR_OBJ_INVALID != m_type );

    m_name = el->Attribute("name");

    DBGX("%s\n",m_name.c_str());

    XMLNode* tmp = m_xmlElement->FirstChild();

    // find the attributes element
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        //DBGX("%s\n",el->Name());

        if ( 0 == strcmp( el->Name(), "attributes") ) {
            tmp = el->FirstChild();
            break;
        }
        tmp = tmp->NextSibling();
    }

    // iterate over the children
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        DBGX("obj=`%s` adding attr=`%s`\n",
                            m_name.c_str(),el->Attribute("name") );

        attrAdd( new _Attr( this, el ) ); 

        tmp = tmp->NextSibling();
    }
    DBGX("return %s\n",m_name.c_str());
}
 
PWR_ObjType _Obj::type()
{
    return m_type;
}

_Obj* _Obj::parent() 
{
    return m_parent; 
}

plugin_dev_t* _Obj::findDev( const std::string name ) {
    return m_ctx->findDev( name );
}

_Obj* _Obj::findChild( std::string name ) 
{
    DBGX("%s\n",name.c_str());
    if ( ! m_children ) {
        m_children = m_ctx->findChildren( m_xmlElement, this );
    }
    
    DBGX("%s\n",name.c_str());
    return m_children->find( name );
}

_Grp* _Obj::children() 
{ 
    if ( ! m_children ) return m_children; 

    return m_children = m_ctx->findChildren( m_xmlElement, this );
}

_Attr* _Obj::attrFindType( PWR_AttrType type )
{
    //DBGX("%d\n", type );
    std::map<int,_Attr*>::iterator iter;

    if ( (iter = m_attrMap.find(type) ) != m_attrMap.end() ) { 
        return iter->second;
    }
    return PWR_NULL;
}
