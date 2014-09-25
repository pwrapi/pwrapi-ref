
#include <iostream>
#include <sstream>

#include <ulxmlrpcpp/ulxr_tcpip_connection.h>
#include <ulxmlrpcpp/ulxr_http_protocol.h>
#include <ulxmlrpcpp/ulxr_requester.h>
#include <ulxmlrpcpp/ulxr_value.h>
#include <ulxmlrpcpp/ulxr_except.h>

#include "objectUrl.h"
#include "util.h"

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

int _ObjUrl::attrGetValue( PWR_AttrName attr, void* buf, size_t len, PWR_Time* ts )
{
  	TcpIpConnection conn (false, m_url, m_port);
  	HttpProtocol prot(&conn);
  	Requester client(&prot);

    MethodCall  rpc( ULXR_PCHAR("attrGetValue") );
	rpc.addParam( Integer( getpid() ) );
	rpc.addParam( RpcString( name() ) );
    rpc.addParam( Integer( attr ) );

    MethodResponse resp = client.call( rpc, ULXR_PCHAR("/RPC2") );

   	Struct xx = resp.getResult();

	int retval = Integer(xx.getMember(ULXR_PCHAR("Retval"))).getInteger(); 

	*(uint64_t*)buf = StringToNum(
        RpcString( xx.getMember(ULXR_PCHAR("Value"))).getString()).uint64();
	*ts = StringToNum(
        RpcString( xx.getMember(ULXR_PCHAR("TimeStamp"))).getString()).uint64();

	return retval;
}

int _ObjUrl::attrSetValue( PWR_AttrName attr, void* buf, size_t len )
{
  	TcpIpConnection conn (false, m_url, m_port);
  	HttpProtocol prot(&conn);
  	Requester client(&prot);

    MethodCall rpc( ULXR_PCHAR("attrSetValue") );
	rpc.addParam(Integer( getpid() ) );
	rpc.addParam( RpcString( name() ) );
    rpc.addParam(Integer( attr ));
    rpc.addParam( RpcString( NumToString( *(uint64_t*)buf) ));

    MethodResponse resp = client.call( rpc, ULXR_PCHAR("/RPC2") );

   	Integer xx = resp.getResult();

	return xx.getInteger();
}

int _ObjUrl::attrGetValues( const std::vector<PWR_AttrName>& attrs, void* buf,
                         std::vector<PWR_Time>& ts, std::vector<int>& status )
{
  	TcpIpConnection conn (false, m_url, m_port);
  	HttpProtocol prot(&conn);
  	Requester client(&prot);

	Array attrs2;
	for ( unsigned int i = 0; i < attrs.size(); i++ ) {
		attrs2.addItem( Integer( attrs[i] ) );
	}

    MethodCall  rpc(ULXR_PCHAR("attrGetValues"));

	rpc.addParam( Integer( getpid() ) );
	rpc.addParam( RpcString( name() ) );
	rpc.addParam( attrs2 );

    MethodResponse resp = client.call( rpc, ULXR_PCHAR("/RPC2") );

   	Struct xx = resp.getResult();

	Array values  = Array( xx.getMember( ULXR_PCHAR("Value") ) ); 
	Array ts2     = Array( xx.getMember( ULXR_PCHAR("TimeStamp") ) );
	Array status2 = Array( xx.getMember( ULXR_PCHAR("Status") ) );

	assert( values.size() == attrs.size() );

	for ( unsigned int i = 0; i < values.size(); i++ ) {
		((uint64_t*)buf)[i] = StringToNum(
                RpcString( values.getItem(i) ).getString() ).uint64(); 
		ts[i] = StringToNum(
                RpcString( ts2.getItem(i) ).getString() ).uint64(); 
	}

    for ( unsigned int i = 0; i < status2.size(); i++ ) {
        Struct yy = Struct( status2.getItem(i) );
        PWR_AttrName name = (PWR_AttrName)
                Integer( yy.getMember(ULXR_PCHAR("Name"))).getInteger();
        int error = Integer( yy.getMember(ULXR_PCHAR("Error"))).getInteger();
        for ( unsigned int j=0; j < attrs.size(); j++ ) {
            if ( name == attrs[j] ) {
		        status[i] = error;
            } else {
		        status[i] = 0;
            }
        }
    }

	return  Integer(xx.getMember(ULXR_PCHAR("Retval"))).getInteger(); 
}

int _ObjUrl::attrSetValues( const std::vector<PWR_AttrName>& attrs, void* buf,
                                            std::vector<int>& status  )
{
  	TcpIpConnection conn (false, m_url, m_port);
  	HttpProtocol prot(&conn);
  	Requester client(&prot);

	Array attrs2;
	Array values2;
	for ( unsigned int i = 0; i < attrs.size(); i++ ) {
		attrs2.addItem( Integer( attrs[i] ) );
		values2.addItem( RpcString(NumToString(((uint64_t*)buf)[i] ) )  );
	}

    MethodCall  rpc(ULXR_PCHAR("attrSetValues"));

	rpc.addParam( Integer( getpid() ) );
	rpc.addParam( RpcString( name() ) );
	rpc.addParam( attrs2 );
	rpc.addParam( values2 );

    MethodResponse resp = client.call( rpc, ULXR_PCHAR("/RPC2") );

   	Struct xx = resp.getResult();

	Array status2 = Array( xx.getMember( ULXR_PCHAR("Status") ) );

    for ( unsigned int i = 0; i < status2.size(); i++ ) {
        Struct yy = Struct( status2.getItem(i) );
        PWR_AttrName name = (PWR_AttrName)
                Integer( yy.getMember(ULXR_PCHAR("Name"))).getInteger();
        int error = Integer( yy.getMember(ULXR_PCHAR("Error"))).getInteger();
        for ( unsigned int j=0; j < attrs.size(); j++ ) {
            if ( name == attrs[j] ) {
		        status[i] = error;
            } else {
		        status[i] = 0;
            }
        }
    }

	return  Integer(xx.getMember(ULXR_PCHAR("Retval"))).getInteger(); 
}
