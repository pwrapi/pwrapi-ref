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
