
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
	void close();

    int getSelectFd( ) { return m_fd; } 
    static TcpEventChannel* selected( int fd ); 

  private:
	int initClient( std::string hostname, std::string port);
	int initServer( std::string port );
    int setupRecv( int port );
	int xx();
    int         m_chan;
    int         m_fd;
	std::string m_clientServer;
	std::string m_clientServerPort;

    static std::map<int,TcpEventChannel*> m_chanFdMap;
};

class TcpChannelSelect : public ChannelSelect {
  public:
    TcpChannelSelect(); 
    virtual bool addChannel( EventChannel*, Data* );
    virtual bool delChannel( EventChannel* );
    virtual Data* wait();

  private:
    std::map<EventChannel*,Data*> m_chanMap;
};

#endif
