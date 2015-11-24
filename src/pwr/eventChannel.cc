#include <tcpEventChannel.h>
#include <iostream>

EventChannel* getEventChannel( const std::string& type,
		EventChannel::AllocFuncPtr func, const std::string& config, 
		const std::string& name )
{
    if ( 0 == type.compare("TCP") ) {
        return new TcpEventChannel( func, config, name );
    }
    return NULL;
}

ChannelSelect* getChannelSelect( const std::string& type )
{
    if ( 0 == type.compare("TCP") ) {
        return new TcpChannelSelect( );
    }
    return NULL;   
}
