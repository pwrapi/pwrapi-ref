
#include "./objectEl.h"
#include "./util.h"
#include "./group.h"

#include "./debug.h"

using namespace tinyxml2;

_ObjEl::_ObjEl( _Cntxt* ctx, _Obj* parent, tinyxml2::XMLElement* el ) :
	_Obj( ctx, parent ),
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

    // find the devices element
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        //DBGX("%s\n",el->Name());

        if ( 0 == strcmp( el->Name(), "devices") ) {
            tmp = el->FirstChild();
            break;
        }
        tmp = tmp->NextSibling();
    }

    // iterate over the devices 
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        const char* name = el->Attribute("name");
        const char* device = el->Attribute("device"); 
        const char* openString = el->Attribute("openString"); 
        DBGX("obj=`%s` adding dev=`%s` device=`%s` openString=`%s`\n",
                            m_name.c_str(), name, device, openString );

        m_devices[ name ] = m_ctx->newDev( device, openString );

        tmp = tmp->NextSibling();
    }


    tmp = m_xmlElement->FirstChild();

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
        m_attrVector[ attr->name() ] = attr; 

        tmp = tmp->NextSibling();
    }
    DBGX("return %s\n",m_name.c_str());
}

_ObjEl::~_ObjEl()
{
    std::map<std::string, _Dev* >::iterator iter = m_devices.begin();
    for ( ; iter != m_devices.end(); ++iter ) {
        delete iter->second;
    }
}

int _ObjEl::attrGetValue( PWR_AttrName name, void* buf, size_t len, PWR_Time* ts )
{
    _Attr* attr = m_attrVector[name];
    if ( attr ) {
        return attr->getValue( buf, len, ts );
    } else {
        return PWR_RET_INVALID;
    }
}

int _ObjEl::attrSetValue( PWR_AttrName name, void* buf, size_t len )
{
    _Attr* attr = m_attrVector[name];
    if ( attr ) {
        return attr->setValue( buf, len );
    } else {
        return PWR_RET_INVALID;
    }
}

struct XX {
    std::vector<PWR_AttrName> attrs;
    std::vector<uint64_t>     data;
    std::vector<PWR_Time>     ts;
    std::vector<int>          status;
    std::vector<int>          foo;
};

int _ObjEl::attrGetValues( const std::vector<PWR_AttrName>& attrs, void* buf,
                         std::vector<PWR_Time>& ts, std::vector<int>& status )
{
    int retval = PWR_RET_SUCCESS;

    std::map< GraphNode*, XX > foobar;
    std::vector< std::vector<void*> > var1( attrs.size() );

    DBGX("%s buf=%p\n",name().c_str(),buf);

    for ( unsigned int i=0; i < attrs.size(); i++ ) {

        _Attr* attr = m_attrVector[ attrs[i] ];

        if ( attr ) {

            for ( unsigned int j = 0; j < attr->foobar().size(); j++ ) {

                DBGX("%s adding src %lu\n",name().c_str(), foobar.size() );

                XX& xx = foobar[ attr->foobar()[j] ];

                xx.attrs.push_back( attrs[i] );
                xx.foo.push_back( i );
            }
        }
    }
 
    std::map<GraphNode*, XX >::iterator iter = foobar.begin();
    for ( ; iter != foobar.end(); ++iter ) {
        XX& xx = (*iter).second;
        unsigned int num = xx.attrs.size();

        xx.data.resize( num );
        xx.ts.resize( num );
        xx.status.resize( num );

        (*iter).first->attrGetValues( xx.attrs, &xx.data[0], xx.ts, xx.status ); 

        for ( unsigned int i = 0; i < num; i++ ) {
            if ( PWR_RET_SUCCESS != xx.status[i] ) {
                retval = PWR_RET_FAILURE;
            }

            var1[ xx.foo[i] ].push_back( &xx.data[i] ); 
        }
    }

    for ( unsigned int i=0; i < attrs.size(); i++ ) {

        DBGX("%s \n",name().c_str());

        _Attr* attr = m_attrVector[ attrs[i] ];

        if ( attr ) {
            if ( ! var1[i].empty() ) {
                attr->op( (void*) &((uint64_t*)buf)[i], var1[i] );
            }
        } else {
            status[i] = PWR_RET_INVALID;
            retval = PWR_RET_FAILURE;
        }
        DBGX("%s \n",name().c_str());
    }
    
    return retval;
}

int _ObjEl::attrSetValues( const std::vector<PWR_AttrName>& attrs, void* buf,
                                            std::vector<int>& status  )
{
    int retval = PWR_RET_SUCCESS;
    DBGX("%s \n",name().c_str());

    std::map< GraphNode*, XX > foobar;

    DBGX("%s \n",name().c_str());

    for ( unsigned int i=0; i < attrs.size(); i++ ) {

        _Attr* attr = m_attrVector[ attrs[i] ];

        if ( attr ) {

            for ( unsigned int j = 0; j < attr->foobar().size(); j++ ) {

                DBGX("%s adding src %lu\n",name().c_str(), foobar.size() );

                XX& xx = foobar[ attr->foobar()[j] ];

                xx.data.push_back( ( ( uint64_t* )buf)[i] );
                xx.attrs.push_back( attrs[i] );
            }
        } 
    }
 
    std::map<GraphNode*, XX >::iterator iter = foobar.begin();
    for ( ; iter != foobar.end(); ++iter ) {
        XX& xx = (*iter).second;
        unsigned int num = xx.attrs.size();

        xx.status.resize( num );
        (*iter).first->attrSetValues( xx.attrs, &xx.data[0], xx.status );

        for ( unsigned int i = 0; i < num; i++ ) {
            if ( PWR_RET_SUCCESS != xx.status[i] ) {
                retval = PWR_RET_FAILURE;
            }
        }
    }

    return retval;
}

_Dev* _ObjEl::findDev( const std::string name )
{
    DBGX("%s\n",name.c_str());
    return m_devices[name];
}

_Obj* _ObjEl::findChild( std::string name ) 
{
    DBGX("%s\n",name.c_str());
    if ( ! m_children ) {
        m_children = m_ctx->findChildren( m_xmlElement, this );
    }
    
    DBGX("%s\n",name.c_str());
    return m_children->find( name );
}

_Grp* _ObjEl::children() 
{ 
    if ( ! m_children ) return m_children; 

    return m_children = m_ctx->findChildren( m_xmlElement, this );
}
