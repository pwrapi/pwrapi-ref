#include <assert.h>
#include "./attribute.h"
#include "./util.h"
#include "./object.h"
#include "./debug.h"

using namespace tinyxml2;


static void fpSum( int num, void* out, void* in );  
static void fpSum2( void* out, const std::vector<void*>& in );  

_Attr::_Attr( _Obj* obj, tinyxml2::XMLElement* el  ) :
    m_obj( obj ),
    m_xml( el ),
    m_op( NULL ),
    m_op2( NULL )
{
    m_type = attrTypeStrToInt( el->Attribute("name") );
    assert( PWR_ATTR_INVALID != m_type );

    DBGX("name=`%s` op=`%s`\n",
                    m_xml->Attribute("name"),m_xml->Attribute("op"));

    m_name = m_xml->Attribute("name");
    m_dataType = attrTypeToDataType( m_type );

    if ( 0 == strcmp( "SUM", m_xml->Attribute("op") ) ) {
        m_op = fpSum;
        m_op2 = fpSum2;
    }

    switch ( m_dataType ) {
      case PWR_ATTR_DATA_FLOAT:
        m_len = sizeof(float);
        break;
      case PWR_ATTR_DATA_INT:
        m_len = sizeof(int);
        break;
      case PWR_ATTR_DATA_STRING:
        m_len = 1024;
        break;
      case PWR_ATTR_DATA_INVALID:
        break;
    }

    if ( 0 != strcmp( m_xml->Attribute("op"), "INVALID" ) ) {
        initSrcList( m_xml );
    }
}

std::vector<Foobar*>& _Attr::foobar()
{
    return m_foobar;
}

void _Attr::op( void* out, const std::vector<void*>& in )
{
    m_op2( out, in );
}

int _Attr::getValue( void* ptr, size_t len, unsigned long long* ts ) 
{
    DBGX("%s %s\n",m_obj->name().c_str(), m_name.c_str());

    if ( len > m_len ) return PWR_ERR_LENGTH; 

    if ( m_foobar.empty() ) {
        return PWR_ERR_INVALID;
    } 

    unsigned char * buf = (unsigned char* )malloc( len * m_foobar.size() );
    for ( unsigned int i = 0; i <  m_foobar.size(); i++ ) {
        
        unsigned long long _ts;
        m_foobar[i]->attrGetValue( m_type, buf + len * i, len, &_ts ); 
        *ts = _ts;
    }

    assert( m_op );
    m_op( m_foobar.size(), ptr, buf );

    return PWR_ERR_SUCCESS;
}

int _Attr::setValue( void* ptr, size_t len )
{
    if ( len > m_len ) return PWR_ERR_LENGTH; 

    if ( m_foobar.empty() ) {
        return PWR_ERR_INVALID;
    } 

    for ( unsigned int i = 0; i <  m_foobar.size(); i++ ) {
        
        m_foobar[i]->attrSetValue( m_type, ptr, len ); 
    }

    return PWR_ERR_SUCCESS;
}

void _Attr::initSrcList( tinyxml2::XMLElement* el )
{
    DBGX("%s name=`%s` op=`%s`\n", m_obj->name().c_str(),
                    m_xml->Attribute("name"),m_xml->Attribute("op"));
    XMLNode* tmp = el->FirstChild();

    // find the attributes element
    while ( tmp ) {
        el = static_cast<XMLElement*>(tmp);

#if 0
        DBGX("attr src type=`%s` name=`%s`\n",
                el->Attribute("type"), el->Attribute("name"));
#endif

        if ( 0 == strcmp( "child", el->Attribute("type" ) ) ) {

            m_foobar.push_back( m_obj->findChild( el->Attribute("name") ) );
            
        } else if ( 0 == strcmp( "plugin", el->Attribute("type" ) ) )  {

            DBGX("plugin name=`%s`\n",el->Attribute("name"));
            m_foobar.push_back( m_obj->findDev( el->Attribute("name"),
                                                el->Attribute("initString") ) );

        }
        tmp = tmp->NextSibling();
    }
#if 0
    DBGX("return\n");
#endif
}

static void fpSum( int num, void* _out, void* _in )
{
    float* out = (float*) _out;
    float* in  = (float*) _in;
    *out = in[0];
     
    //printf("%s() i=%d val=%f\n",__func__, 0, in[0]);
    for ( int i = 1; i < num ;i++  ) {
        //printf("%s() i=%d val=%f\n",__func__, i, in[i]);
        *out += in[i];
    }
}

static void fpSum2( void* out, const std::vector<void*>& in  )
{
    *((float*) out) = *((float*) in[0]);

    printf("%s() i=%d val=%f\n",__func__, 0, *((float*) out));
//    out.timeStamp = in[0]->timeStamp;

    for ( unsigned int i=1; i < in.size(); i++ ) {
        printf("%s() i=%d val=%f\n",__func__, i, *((float*) in[i]));
        *((float*) out) += *((float*) in[i]);
    } 
}
