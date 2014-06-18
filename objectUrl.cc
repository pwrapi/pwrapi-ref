
#include <iostream>
#include <sstream>

#include <ulxmlrpcpp/ulxr_tcpip_connection.h>
#include <ulxmlrpcpp/ulxr_http_protocol.h>
#include <ulxmlrpcpp/ulxr_requester.h>
#include <ulxmlrpcpp/ulxr_value.h>
#include <ulxmlrpcpp/ulxr_except.h>

#include "./objectUrl.h"
#include "./util.h"

using namespace ulxr;

_ObjUrl::_ObjUrl( _Cntxt* ctx, _Obj* parent,
                    std::string name, std::string url ) :
    _Obj( ctx, parent )
{
    m_name = name;
	m_url = url.substr(0, url.find_first_of( ":") );
	m_port = atoi( url.substr( url.find_first_of( ":") + 1, -1 ).c_str() ); 
    printf("url=%s %d\n",m_url.c_str(),m_port);
}

int _ObjUrl::attrGetValue( PWR_AttrName name, void* buf, size_t len, PWR_Time* ts )
{
  	TcpIpConnection conn (false, m_url, m_port);
  	HttpProtocol prot(&conn);
  	Requester client(&prot);

    MethodCall  attrGetValue(ULXR_PCHAR("attrGetValue"));
	attrGetValue.addParam(Integer( getpid() ) );
    attrGetValue.addParam(Integer( name ));

    MethodResponse resp = client.call( attrGetValue, ULXR_PCHAR("/RPC2") );

   	Struct xx = resp.getResult();

	int retval = Integer(xx.getMember(ULXR_PCHAR("Retval"))).getInteger(); 

	*(uint64_t*)buf = StringToNum( RpcString( xx.getMember(ULXR_PCHAR("Value"))).getString()).uint64();
	*ts = StringToNum( RpcString( xx.getMember(ULXR_PCHAR("TimeStamp"))).getString()).uint64();

	return retval;
}
int _ObjUrl::attrSetValue( PWR_AttrName name, void* buf, size_t len )
{
  	TcpIpConnection conn (false, m_url, m_port);
  	HttpProtocol prot(&conn);
  	Requester client(&prot);

    MethodCall  attrGetValue(ULXR_PCHAR("attrSetValue"));
	attrGetValue.addParam(Integer( getpid() ) );
    attrGetValue.addParam(Integer( name ));
    attrGetValue.addParam( RpcString( NumToString( *(uint64_t*)buf) ));

    MethodResponse resp = client.call( attrGetValue, ULXR_PCHAR("/RPC2") );

   	Integer xx = resp.getResult();

	return xx.getInteger();
}
int _ObjUrl::attrGetValues( const std::vector<PWR_AttrName>& attrs, void* buf,
                         std::vector<PWR_Time>& ts, std::vector<int>& status )
{
    assert(0);
}

int _ObjUrl::attrSetValues( const std::vector<PWR_AttrName>& attrs, void* buf,
                                            std::vector<int>& status  )
{
    assert(0);
}
