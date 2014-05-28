
#include "cntxt.h"
#include "debug.h"

using namespace tinyxml2;


_Cntxt::_Cntxt( PWR_CntxtType type, PWR_Role role, const char* name  ) :
            m_top( NULL )
{
    DBGX("enter\n");
    // find data base
    // verify type
    // verify role
    // who am I ?
    m_configFile = "systemX.xml";
    m_topName    = "plat.cab0.board0.node0";

    m_xml = new tinyxml2::XMLDocument();
    m_xml->LoadFile( m_configFile.c_str() );
    assert( tinyxml2::XML_SUCCESS == m_xml->ErrorID() );
    //printTree( m_xml->RootElement() );
    DBGX("return\n");
}

_Obj* _Cntxt::getSelf() {

    XMLElement* el = XMLFindObject( m_topName );
    assert(el);

    _Obj* obj = new _Obj( this, NULL, el );
    return obj;
}

_Grp* _Cntxt::findChildren( XMLElement* el, _Obj* parent )
{
    _Grp* grp = NULL;
    const std::string name = el->Attribute("name"); 

    XMLNode* tmp = el->FirstChild(); 

    // find the children element
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        //DBGX("%s\n",el->Name());

        if ( 0 == strcmp( el->Name(), "children") ) {
            tmp = el->FirstChild();
            if ( tmp ) {
                grp = new _Grp(this); 
            }
            break;
        }
        tmp = tmp->NextSibling();
    }

    // iterate over the children
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

        DBGX("%s\n", el->Attribute("name"));
        XMLElement* child = XMLFindObject( name + "." + el->Attribute("name") );
        assert( child );

        grp->add( new _Obj( this, parent, child ) );
        DBGX("%s done\n", el->Attribute("name"));

        tmp = tmp->NextSibling();
    }
    
    DBGX("return\n");
    return grp;
}

XMLElement* _Cntxt::XMLFindObject( const std::string name )
{
#if 0
    printf("%s() `%s`\n",__func__,name.c_str());
#endif
    XMLNode* tmp = m_xml->RootElement()->FirstChild(); 

    while ( tmp ) {
        XMLElement* el = static_cast<XMLElement*>(tmp);

#if 0
        printf("%s %s name=`%s`\n",el->Name(),
                el->Attribute("type"), el->Attribute("name"));
#endif

        if ( 0 == name.compare( el->Attribute("name") ) ) {
            return el;
        }
        tmp = tmp->NextSibling();

    }

    return NULL;
}

void _Cntxt::printTree( XMLNode* node )
{
    XMLElement* el = static_cast<XMLElement*>(node);

    if ( NULL == el ) return;

    if ( ! strcmp(el->Name(),"obj") ) {
        DBGX("%s %s name=`%s`\n",el->Name(),
                el->Attribute("type"),
                el->Attribute("name"));
    } else if ( ! strcmp(el->Name(),"child") ) {
        DBGX("%s %s\n",el->Name(),el->Attribute("name"));
    } else if ( ! strcmp(el->Name(),"attr") ) {
        DBGX("%s %s\n",el->Name(),el->Attribute("type"));
    } else {
       DBGX("%s \n",el->Name());
    }

    if ( ! node->NoChildren() ) {
        XMLNode* tmp = node->FirstChild();
        do {
            printTree( tmp );
        } while ( ( tmp = tmp->NextSibling() ) );
    }
}
