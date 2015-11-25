
#ifndef _SERVER_H
#define _SERVER_H

#include <deque>
#include <map>
#include <pwr.h>
#include <eventChannel.h>
#include <events.h>
#include "debug.h"

class EventChannel;

namespace PWR_Server { 

struct CommInfo {
	std::vector<PWR_Obj> objects;
};

struct Args {
    std::string port;
    std::string host;
    std::string topObj;

	std::string pwrApiConfig;
	std::string pwrApiRoot;
	std::string pwrApiLocation;
	std::string pwrApiServer;
	std::string pwrApiServerPort;
};

class Server : public EventGenerator {
  public:

	Server( int, char* [] );
	~Server();
	int work();

	std::map< CommID, CommInfo > 	m_commMap;
	PWR_Cntxt m_ctx;

  private:
	ChannelSelect*  m_chanSelect;
	Args			m_args;
};

class SelectData : public ChannelSelect::Data {
  public:
    SelectData ( EventChannel* chan ) :
        m_chan( chan )
    { }
    virtual bool process( Server* ) = 0;

  protected:
    EventChannel* m_chan;
};

class RouterData : public SelectData {
  public:
    RouterData(  EventChannel* chan ) :
        SelectData( chan )
    { }
    bool process( Server* gen ) {
        Event* event = m_chan->getEvent();
		if ( NULL == event ) {
			return true;
		} else { 
        	if ( event->process( gen, m_chan ) ) { 
				DBG("\n");
				delete event;	
			}	
			return false;
		}
    }
};

class CntxtData : public SelectData {
  public:
    CntxtData(  EventChannel* chan ) :
        SelectData( chan )
    { }

    bool process( Server* gen ) {
        if ( PWR_RET_SUCCESS == PWR_CntxtMakeProgress( gen->m_ctx ) ) {
        	return false;
		} else {
        	return true;
		}
    }
};

}

#endif
