
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

    m_attrVector.resize(PWR_ATTR_INVALID);

    for ( int i = 0; i < PWR_ATTR_INVALID; i++ ) {
        m_attrVector[i] = NULL;
    }

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

        _Attr* attr = new _Attr( this, el );
        m_attrVector[ attr->type() ] = attr; 

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

int _Obj::attrGetValue( PWR_AttrType type, void* ptr, size_t len, PWR_Time* ts )
{
    _Attr* attr = m_attrVector[type];
    if ( attr ) {
        return attr->getValue( ptr, len, ts );
    } else {
        return PWR_ERR_INVALID;
    }
}

int _Obj::attrSetValue( PWR_AttrType type, void* ptr, size_t len )
{
    _Attr* attr = m_attrVector[type];
    if ( attr ) {
        return attr->setValue( ptr, len );
    } else {
        return PWR_ERR_INVALID;
    }
}

struct XX {
    std::vector< PWR_Value >  value;
    std::vector< int >        status;
    std::vector< std::vector< unsigned char> > buf;
};

int _Obj::attrGetValues( int num, PWR_Value v[], int status[] )
{
    int retval = PWR_ERR_SUCCESS;
    std::map< Foobar*, XX > foobar;
    std::vector< std::vector<PWR_Value*> > var1( num );

    DBGX("%s \n",name().c_str());

    for ( int i=0; i < num; i++ ) {

        _Attr* attr = m_attrVector[ v[i].type ];

        if ( attr ) {

            for ( unsigned int j = 0; j < attr->foobar().size(); j++ ) {

                DBGX("%s adding src %lu\n",name().c_str(), foobar.size() );

                XX& xx = foobar[ attr->foobar()[j] ];

                xx.status.push_back( 0 );
                xx.buf.push_back( std::vector<unsigned char>(v[i].len) );

                PWR_Value u;
                u.type = v[i].type;
                u.len =  v[i].len; 
                u.ptr =  &xx.buf.back()[0];

                xx.value.push_back( u );

                var1[i].push_back( &xx.value.back() );
            }
        }
    }
 
    std::map<Foobar*, XX >::iterator iter = foobar.begin();
    for ( ; iter != foobar.end(); ++iter ) {
        XX& xx = (*iter).second;

        (*iter).first->attrGetValues( xx.value.size(), 
                                    &xx.value[0], &xx.status[0] );

        for ( unsigned int i = 0; i < xx.value.size(); i++ ) {
            if ( PWR_ERR_SUCCESS != xx.status[i] ) {
                retval = PWR_ERR_FAILURE;
            }
        }
    }

    for ( int i=0; i < num; i++ ) {

        _Attr* attr = m_attrVector[ v[i].type ];

        if ( attr ) {
            if ( !  var1.empty() ) {
                attr->op( v[i], var1[i] );
            }
        } else {
            status[i] = PWR_ERR_INVALID;
            retval = PWR_ERR_FAILURE;
        }
    }
    
    return retval;
}

int _Obj::attrSetValues( int num, PWR_Value v[], int status[] )
{
    int retval = PWR_ERR_SUCCESS;
    DBGX("%s \n",name().c_str());

    std::map< Foobar*, XX > foobar;

    DBGX("%s \n",name().c_str());

    for ( int i=0; i < num; i++ ) {

        _Attr* attr = m_attrVector[ v[i].type ];

        if ( attr ) {

            for ( unsigned int j = 0; j < attr->foobar().size(); j++ ) {

                DBGX("%s adding src %lu\n",name().c_str(), foobar.size() );

                XX& xx = foobar[ attr->foobar()[j] ];

                xx.status.push_back( 0 );

                xx.value.push_back( v[i] );
            }
        } 
    }
 
    std::map<Foobar*, XX >::iterator iter = foobar.begin();
    for ( ; iter != foobar.end(); ++iter ) {
        XX& xx = (*iter).second;

        (*iter).first->attrSetValues( xx.value.size(), 
                                    &xx.value[0], &xx.status[0] );
        for ( unsigned int i = 0; i < xx.value.size(); i++ ) {
            if ( PWR_ERR_SUCCESS != xx.status[i] ) {
                retval = PWR_ERR_FAILURE;
            }
        }
    }

    return retval;
}

_Dev* _Obj::findDev( const std::string name, const std::string config )
{
    return m_ctx->findDev( name, config );
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
