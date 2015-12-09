
#ifndef _DIST_CNTXT_H
#define _DIST_CNTXT_H

#include <vector>
#include <map>

#include "cntxt.h"
#include "pwrdev.h"

namespace PowerAPI {

class Config;
class Communicator;
class Device;

class DistCntxt : public Cntxt {

  public:
    DistCntxt( PWR_CntxtType type, PWR_Role role, const char* name );
    ~DistCntxt( ) {}
	EventChannel* getEventChannel() { return m_evChan; }

	int makeProgress();
	AttrInfo* initAttr( Object*, PWR_AttrName );
	virtual Object* createObject( std::string, PWR_ObjType, Cntxt* );

  private:
	void traverse( std::string objName, PWR_AttrName,
					std::vector<Device*>&, std::set<Object*>& );

	Communicator* getCommunicator( std::set<Object*> objects );
	void initPlugins( Config& cfg );
	bool initDevice( std::string& devName );
	plugin_dev_t* getDev( std::string lib, std::string name );

	EventChannel*   initEventChannel();
	EventChannel*   m_evChan;

    std::map< std::string, plugin_dev_t* >     m_pluginLibMap;
    std::map< std::string, std::pair< plugin_dev_t*, plugin_devops_t* > > m_devMap;
	std::map< plugin_devops_t*, std::map< std::string, Device* > > m_deviceMap;

	std::map< std::set< Object*>, Communicator* >	m_commMap;
	std::string m_myLocation;	
};

}

#endif

