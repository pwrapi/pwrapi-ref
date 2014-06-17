
#include <ulxmlrpcpp/ulxmlrpcpp.h>

#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include <ulxmlrpcpp/ulxr_except.h>
//#include <ulxmlrpcpp/ulxr_signature.h>
#include <ulxmlrpcpp/ulxr_dispatcher.h>
#include <ulxmlrpcpp/ulxr_http_protocol.h>
#include <ulxmlrpcpp/ulxr_tcpip_connection.h>
//#include <ulxmlrpcpp/ulxr_mtrpc_server.h>

#include "../util.h"
#include "../pow.h"

using namespace ulxr;

std::map<int, PWR_Obj> _objMap;

PWR_Obj findObj( int pid )
{
	if ( _objMap.find( pid ) == _objMap.end() ) {
		PWR_Cntxt ctx;
		ctx = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_ADMIN, "Daemon" );		
		assert(ctx);

		_objMap[pid] = PWR_CntxtGetSelf( ctx );
		assert( _objMap[pid] );
	}

	return _objMap[pid];
}

MethodResponse attrGetValue(const MethodCall &calldata)
{
	if (calldata.numParams() != 2)
    throw ParameterException(InvalidMethodParameterError,
                             ULXR_PCHAR("Exactly 1 parameter allowed for \"")
                             +calldata.getMethodName() + ULXR_PCHAR("\""));

	int pid = Integer( calldata.getParam(0) ).getInteger(); 
	PWR_AttrName name = 
				(PWR_AttrName)Integer( calldata.getParam(1) ).getInteger(); 

	std::cout << __func__ << "() " << pid << " " << 
				attrNameToString( name ) << "\n"; 

	PWR_Obj obj = findObj( pid );

	uint64_t v;
	PWR_Time ts;
	int retval = PWR_ObjAttrGetValue( obj, name, &v, &ts ); 

	Struct resp;
  	resp.addMember(ULXR_PCHAR("Value"), RpcString(NumToString(v) ) );
  	resp.addMember(ULXR_PCHAR("TimeStamp"), RpcString(NumToString(ts) ) );
  	resp.addMember(ULXR_PCHAR("Retval"), Integer( retval ) );

  	return MethodResponse (resp);;
}

MethodResponse attrSetValue(const MethodCall &calldata)
{
	if (calldata.numParams() != 3)
    throw ParameterException(InvalidMethodParameterError,
                             ULXR_PCHAR("Exactly 1 parameter allowed for \"")
                             +calldata.getMethodName() + ULXR_PCHAR("\""));

	int pid = Integer( calldata.getParam(0) ).getInteger(); 
	PWR_AttrName name = 
				(PWR_AttrName)Integer( calldata.getParam(1) ).getInteger(); 

	std::cout << __func__ << "() " << pid << " " << 
				attrNameToString( name ) << "\n"; 

	PWR_Obj obj = findObj( pid );

	uint64_t v = StringToNum( 
			RpcString(calldata.getParam(2)).getString() ).uint64();

	int retval = PWR_ObjAttrSetValue( obj, name, &v ); 

  	return MethodResponse (Integer(retval));;
}

#if 0
MethodResponse attrGetValues(const MethodCall &calldata)
{
	if (calldata.numParams() != 2)
    throw ParameterException(InvalidMethodParameterError,
                             ULXR_PCHAR("Exactly 1 parameter allowed for \"")
                             +calldata.getMethodName() + ULXR_PCHAR("\""));

	int pid = Integer( calldata.getParam(0) ).getInteger(); 
	Array names = calldata.getParam(1);

	std::vector<PWR_AttrName> names2( names.size() ); 
	for ( unsigned int i = 0; i < names.size(); i++ ) { 
		names2[i] = (PWR_AttrName)Integer( names.getItem(i) ).getInteger(); 
		std::cout << __func__ << "() " << pid << " " << 
				attrNameToString( names2[i] ) << "\n"; 
	}


	PWR_Obj obj = findObj( pid );

	std::vector<uint64_t> v( names.size() );
	std::vector<PWR_Time> ts( names.size() );
	std::vector<int>      status( names.size() );
	int retval = PWR_ObjAttrGetValues( obj, names2, &v[0], ts, status  ); 

	Struct resp;
  	resp.addMember(ULXR_PCHAR("Value"), RpcString(NumToString(v) ) );
  	resp.addMember(ULXR_PCHAR("TimeStamp"), RpcString(NumToString(ts) ) );
  	resp.addMember(ULXR_PCHAR("Retval"), Integer( retval ) );

  	return MethodResponse (resp);;
}
#endif


int main( int argc, char* argv[] )
{
	TcpIpConnection conn (true, 0x7f000001, 32000); 
  	HttpProtocol prot(&conn);
  	Dispatcher server(&prot);

    server.addMethod(&attrGetValue,
			 Struct::getValueName(),
             ULXR_PCHAR("attrGetValue"),
			 Integer::getValueName() + ULXR_PCHAR(",") +
			 Integer::getValueName(),
             ULXR_PCHAR("get the value and timestamp"));

    server.addMethod(&attrSetValue,
			 Integer::getValueName(),
             ULXR_PCHAR("attrSetValue"),
			 Integer::getValueName() + ULXR_PCHAR(",") +
			 Integer::getValueName() + ULXR_PCHAR(",") +
			 RpcString::getValueName(),
             ULXR_PCHAR("get the value and timestamp"));

#if 0
    server.addMethod(&attrGetValues,
			 Struct::getValueName(),
             ULXR_PCHAR("attrGetValues"),
			 Integer::getValueName() + ULXR_PCHAR(",") +
			 Array::getValueName(),
             ULXR_PCHAR("get the value and timestamp"));
#endif


	while(1) {
  		MethodCall call = server.waitForCall();
  		MethodResponse resp = server.dispatchCall( call );
  		server.sendResponse( resp );
		prot.close();
	}

    return 0;
}
