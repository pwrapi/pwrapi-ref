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

#ifndef _DIST_CNTXT_H
#define _DIST_CNTXT_H

#include <vector>
#include <map>

#include "cntxt.h"
#include "pwrdev.h"

class EventChannel;
namespace PowerAPI {

class Config;
class Communicator;
class Device;

class DistCntxt : public Cntxt {

  public:
    DistCntxt( PWR_CntxtType type, PWR_Role role, const char* name );
    ~DistCntxt( );
	EventChannel* getEventChannel() { return m_evChan; }

	int makeProgress();
	AttrInfo* initAttr( Object*, PWR_AttrName );
	virtual Object* createObject( std::string, PWR_ObjType, Cntxt* );

    virtual Grp*    createGrp( std::string );
    virtual int     destroyGrp( Grp* );

  private:
	void traverse( std::string objName, PWR_AttrName,
					std::vector<Device*>&, std::set<std::string>& );


	Communicator* getCommunicator( std::set<std::string> objects );
	void initPlugins( Config& cfg );
	bool initDevice( std::string& devName );
	plugin_dev_t* getDev( std::string lib, std::string name );

	EventChannel*   initEventChannel();
	EventChannel*   m_evChan;

    std::map< std::string, plugin_dev_t* >     m_pluginLibMap;
    std::map< std::string, std::pair< plugin_dev_t*, plugin_devops_t* > > m_devMap;
	std::map< plugin_devops_t*, std::map< std::string, Device* > > m_deviceMap;

	std::map< std::set< std::string>, Communicator* >	m_commMap;
	std::string m_rootName;	
	std::string m_name;
};

}

#endif

