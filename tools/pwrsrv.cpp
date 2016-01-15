/* 
 * Copyright 2014-2016 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this work 
 * by or on behalf of the U.S. Government. Export of this program may require
 * a license from the United States Government.
 *
 * This file is part of the Power API Prototype software package. For license
 * information, see the LICENSE file in the top level directory of the
 * distribution.
*/


#include <ulxmlrpcpp/ulxmlrpcpp.h>

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include <ulxmlrpcpp/ulxr_except.h>
#include <ulxmlrpcpp/ulxr_dispatcher.h>
#include <ulxmlrpcpp/ulxr_http_protocol.h>
#include <ulxmlrpcpp/ulxr_tcpip_connection.h>

#include "util.h"
#include "pwr.h"
#include "./debug.h"

using namespace ulxr;

std::map< int, PWR_Obj > _objMap;


class NumToString : public std::string  {
  public:
    NumToString( uint64_t num ) {
        std::stringstream tmpSS;
        tmpSS << num;
        append( tmpSS.str() );
    }
};   

class StringToNum {
  public:
    StringToNum( const std::string str ) {
        std::stringstream( str ) >> num;
    }
    uint64_t uint64() {
        return num;
    }
  private:
    uint64_t  num;
};

PWR_Obj findObj( PWR_Obj obj, std::string name )
{
    DBG("name=`%s`\n",name.c_str());
	if ( 0 ==  name.compare( PWR_ObjGetName( obj ) ) ) {
		return obj;
	}
	return NULL;
}

PWR_Obj findObj( int pid, std::string name )
{
    DBG("pid=%d `%s`\n",pid,name.c_str());
	if ( _objMap.find( pid ) == _objMap.end() ) {
        DBG("new\n");
		PWR_Cntxt ctx;
		ctx = PWR_CntxtInit( PWR_CNTXT_DEFAULT, PWR_ROLE_ADMIN, "Daemon" );		
		assert(ctx);

		_objMap[pid] = PWR_CntxtGetEntryPoint( ctx );
		assert( _objMap[pid] );
	}

	return findObj( _objMap[pid], name );
}

MethodResponse attrGetValue(const MethodCall &calldata)
{
	if (calldata.numParams() != 3)
    throw ParameterException(InvalidMethodParameterError,
                             ULXR_PCHAR("Exactly 1 parameter allowed for \"")
                             +calldata.getMethodName() + ULXR_PCHAR("\""));

	int pid = Integer( calldata.getParam(0) ).getInteger(); 
	std::string objName = RpcString( calldata.getParam(1) ).getString();
	PWR_AttrName name = 
				(PWR_AttrName)Integer( calldata.getParam(2) ).getInteger(); 

    DBG("pid=%d `%s`\n",pid,attrNameToString( name ));

	PWR_Obj obj = findObj( pid, objName );
	assert(obj);

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
	if (calldata.numParams() != 4)
    throw ParameterException(InvalidMethodParameterError,
                             ULXR_PCHAR("Exactly 1 parameter allowed for \"")
                             +calldata.getMethodName() + ULXR_PCHAR("\""));

	int pid = Integer( calldata.getParam(0) ).getInteger(); 
	std::string objName = RpcString( calldata.getParam(1) ).getString();
	PWR_AttrName name = 
				(PWR_AttrName)Integer( calldata.getParam(2) ).getInteger(); 

    DBG("pid=%d `%s`\n",pid,attrNameToString( name ));

	PWR_Obj obj = findObj( pid, objName );

	uint64_t v = StringToNum( 
			RpcString(calldata.getParam(3)).getString() ).uint64();

	int retval = PWR_ObjAttrSetValue( obj, name, &v ); 

  	return MethodResponse (Integer(retval));;
}

MethodResponse attrGetValues(const MethodCall &calldata)
{
	if (calldata.numParams() != 3)
    throw ParameterException(InvalidMethodParameterError,
                             ULXR_PCHAR("Exactly 1 parameter allowed for \"")
                             +calldata.getMethodName() + ULXR_PCHAR("\""));

	int pid = Integer( calldata.getParam(0) ).getInteger(); 
	std::string objName = RpcString( calldata.getParam(1) ).getString();
	Array names = calldata.getParam(2);

	std::vector<PWR_AttrName> names2( names.size() ); 

	for ( unsigned int i = 0; i < names.size(); i++ ) { 
		names2[i] = (PWR_AttrName)Integer( names.getItem(i) ).getInteger(); 
        DBG("pid=%d `%s`\n",pid, attrNameToString( names2[i] )); 
	}

	PWR_Obj obj = findObj( pid, objName );

	std::vector<uint64_t> v( names.size() );
	std::vector<PWR_Time> ts( names.size() );

	PWR_Status status = PWR_StatusCreate();
	int retval = PWR_ObjAttrGetValues( obj, names.size(), &names2[0], 
				&v[0], &ts[0], status  ); 

	Struct resp;
	Array vArray;
	Array tsArray;
	Array statusArray;

	if ( retval != PWR_RET_SUCCESS ) { 	
        PWR_AttrAccessError error;
		while ( PWR_StatusPopError( status, &error ) ) {
            Struct xxx;
		    xxx.addMember( ULXR_PCHAR("Error"),Integer( error.error ) );
		    xxx.addMember( ULXR_PCHAR("Name"),Integer( error.name ) );
            statusArray.addItem( xxx );
		}
	}

	PWR_StatusDestroy( status );

	for ( unsigned int i = 0; i < names.size(); i++ ) { 
		vArray.addItem( RpcString(NumToString(v[i])) );
		tsArray.addItem( RpcString(NumToString(ts[i])) );
	}

  	resp.addMember(ULXR_PCHAR("Value"), Array(vArray) );
	resp.addMember(ULXR_PCHAR("TimeStamp"), Array(tsArray) );
  	resp.addMember(ULXR_PCHAR("Status"), Array( statusArray ) );
  	resp.addMember(ULXR_PCHAR("Retval"), Integer( retval ) );

  	return MethodResponse (resp);;
}

MethodResponse attrSetValues(const MethodCall &calldata)
{
	if (calldata.numParams() != 4)
    throw ParameterException(InvalidMethodParameterError,
                             ULXR_PCHAR("Exactly 1 parameter allowed for \"")
                             +calldata.getMethodName() + ULXR_PCHAR("\""));

	int pid = Integer( calldata.getParam(0) ).getInteger(); 
	std::string objName = RpcString( calldata.getParam(1) ).getString();
	Array names = calldata.getParam(2);
	Array values = calldata.getParam(3);

	std::vector<PWR_AttrName> names2( names.size() ); 
	std::vector<uint64_t> values2( names.size() ); 

	for ( unsigned int i = 0; i < names.size(); i++ ) { 
		names2[i] = (PWR_AttrName)Integer( names.getItem(i) ).getInteger(); 
		values2[i] =  StringToNum(
           RpcString( values.getItem(i)).getString()).uint64();
        DBG("pid=%d `%s`\n",pid, attrNameToString( names2[i] )); 
	}

	PWR_Obj obj = findObj( pid, objName );

	PWR_Status status = PWR_StatusCreate();
	int retval = PWR_ObjAttrSetValues( obj, names.size(), &names2[0], 
				&values2[0], status  ); 

	Struct resp;
	Array vArray;
	Array tsArray;
	Array statusArray;

	if ( retval != PWR_RET_SUCCESS ) { 	
        PWR_AttrAccessError error;
		while ( PWR_StatusPopError( status, &error ) ) {
            Struct xxx;
		    xxx.addMember( ULXR_PCHAR("Error"),Integer( error.error ) );
		    xxx.addMember( ULXR_PCHAR("Name"),Integer( error.name ) );
            statusArray.addItem( xxx );
		}
	}

	PWR_StatusDestroy( status );

  	resp.addMember(ULXR_PCHAR("Status"), Array( statusArray ) );
  	resp.addMember(ULXR_PCHAR("Retval"), Integer( retval ) );

  	return MethodResponse (resp);;
}

int main( int argc, char* argv[] )
{
    #define BUF_LEN 1000
    char nameBuf[BUF_LEN];
    char* name; 
    if ( argc == 1 ) {
        int ret = gethostname( nameBuf, BUF_LEN );
        assert( 0 == ret );
        name = nameBuf;
    } else if ( argc == 2 ) {
        name = argv[1];
    } else {
        assert(0);
    }

    struct hostent* phostent = gethostbyname( name );
    assert( phostent );

    unsigned int addr;
    addr =  (unsigned char) phostent->h_addr_list[0][0] << 24;
    addr |= (unsigned char) phostent->h_addr_list[0][1] << 16;
    addr |= (unsigned char) phostent->h_addr_list[0][2] << 8;
    addr |= (unsigned char) phostent->h_addr_list[0][3];
    printf("host=%s addr=%x\n", name, addr );
    
	TcpIpConnection conn (true, addr, 32000); 
  	HttpProtocol prot(&conn);
  	Dispatcher server(&prot);

    server.addMethod(&attrGetValue,
			 Struct::getValueName(),
             ULXR_PCHAR("attrGetValue"),
			 Integer::getValueName() + ULXR_PCHAR(",") +
			 RpcString::getValueName() + ULXR_PCHAR(",") +
			 Integer::getValueName(),
             ULXR_PCHAR("get the value and timestamp"));

    server.addMethod(&attrSetValue,
			 Integer::getValueName(),
             ULXR_PCHAR("attrSetValue"),
			 Integer::getValueName() + ULXR_PCHAR(",") +
			 RpcString::getValueName() + ULXR_PCHAR(",") +
			 Integer::getValueName() + ULXR_PCHAR(",") +
			 RpcString::getValueName(),
             ULXR_PCHAR("get the value and timestamp"));

    server.addMethod(&attrGetValues,
			 Struct::getValueName(),
             ULXR_PCHAR("attrGetValues"),
			 Integer::getValueName() + ULXR_PCHAR(",") +
			 RpcString::getValueName() + ULXR_PCHAR(",") +
			 Array::getValueName(),
             ULXR_PCHAR("get the value and timestamp"));

    server.addMethod(&attrSetValues,
			 Struct::getValueName(),
             ULXR_PCHAR("attrSetValues"),
			 Integer::getValueName() + ULXR_PCHAR(",") +
			 RpcString::getValueName() + ULXR_PCHAR(",") +
			 Array::getValueName() + ULXR_PCHAR(",") +
			 Array::getValueName(),
             ULXR_PCHAR("get the value and timestamp"));

	while(1) {
  		MethodCall call = server.waitForCall();
  		MethodResponse resp = server.dispatchCall( call );
  		server.sendResponse( resp );
		prot.close();
	}

    return 0;
}
