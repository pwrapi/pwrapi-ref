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

#ifndef _TCP_EVENT_CHANNEL_H
#define _TCP_EVENT_CHANNEL_H

#include <sys/select.h> 
#include <string>
#include <map>
#include <set>
#include <eventChannel.h>

class TcpEventChannel : public EventChannel {
  public:
    TcpEventChannel( AllocFuncPtr, std::string config, std::string name = "" );
    TcpEventChannel( AllocFuncPtr, int fd, std::string name = "" );
    ~TcpEventChannel();

    virtual Event* getEvent( bool blocking = true );
    virtual bool sendEvent( Event* );
	EventChannel *accept(  );

    int getFd( ) { return m_fd; } 

  private:
	int initClient( std::string hostname, std::string port);
	int initServer( std::string port );
    int setupRecv( int port );
	int xx();
    int         m_fd;
	std::string m_clientServer;
	std::string m_clientServerPort;
};

class TcpChannelSelect : public ChannelSelect {
  public:
    TcpChannelSelect(); 
    virtual bool addChannel( EventChannel*, Data* );
    virtual bool delChannel( EventChannel* );
    virtual Data* wait();

  private:
	int select(int nfds, fd_set* readfds, fd_set* writefds,
					fd_set* errorfds,struct timeval * timeout);
    std::map<EventChannel*,Data*> m_chanMap;
};

#endif
