
#include "./object.h"
#include "./util.h"


using namespace tinyxml2;

_Obj::_Obj( _Cntxt* ctx, tinyxml2::XMLElement* el ) :
    m_ctx(ctx),
    m_xmlElement(el),
    m_parent( NULL ),
    m_children( NULL )
{
    m_type = objTypeStrToInt( m_xmlElement->Attribute("type") );

    assert( PWR_OBJ_INVALID != m_type );

    m_name = el->Attribute("name");

    XMLNode* tmp = m_xmlElement->FirstChild();

    // find the attributes element
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        //printf("%s\n",el->Name());

        if ( 0 == strcmp( el->Name(), "attributes") ) {
            tmp = el->FirstChild();
            break;
        }
        tmp = tmp->NextSibling();
    }

    // iterate over the children
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        printf("obj=`%s` adding attr=`%s`\n",
                            m_name.c_str(),el->Attribute("name") );

        attrAdd( new _Attr( this, el ) ); 

        tmp = tmp->NextSibling();
    }
}
 
PWR_ObjType _Obj::type()
{
    return m_type;
}

_Obj* _Obj::parent() 
{
    return m_parent; 
}

_Grp* _Obj::children() 
{ 
    if ( m_children ) return m_children; 

    return m_children = m_ctx->findChildren( this->m_xmlElement );
}

_Attr* _Obj::findAttrType( PWR_AttrType type )
{
    std::map<int,_Attr*>::iterator iter;

    if ( (iter = m_attrMap.find(type) ) != m_attrMap.end() ) { 
        return iter->second;
    }
    return PWR_NULL;
}
