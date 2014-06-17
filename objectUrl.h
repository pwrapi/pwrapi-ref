#ifndef _PWR_OBJECTURL_H 
#define _PWR_OBJECTURL_H

#include <ulxmlrpcpp/ulxr_tcpip_connection.h>
#include <ulxmlrpcpp/ulxr_http_protocol.h>
#include <ulxmlrpcpp/ulxr_requester.h>


#include "./object.h" 

struct _ObjUrl : public _Obj
{
  public:
    _ObjUrl( _Cntxt* ctx, _Obj* parent, std::string name, std::string url );
    virtual int attrGetValues( const std::vector<PWR_AttrName>& types,
            void* ptr, std::vector<PWR_Time>& ts, std::vector<int>& status );
    virtual int attrSetValues(  const std::vector<PWR_AttrName>& types,
            void* ptr, std::vector<int>& status );

    virtual int attrGetValue( PWR_AttrName, void*, size_t, PWR_Time* );
    virtual int attrSetValue( PWR_AttrName, void*, size_t );

  private:
    std::string m_url;
	int         m_port;
};

#endif
