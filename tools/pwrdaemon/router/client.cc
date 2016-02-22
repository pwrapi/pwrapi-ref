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

#include "router.h"

using namespace PWR_Router;

Router::Client::Client( Router& rtr ) :m_rtr(rtr) {
	DBGX("\n");
}
Router::Client::~Client() {
	DBGX("\n");
    std::map<CommID,CommCreateEvent* >::iterator iter;
    for ( iter = m_commMap.begin(); iter != m_commMap.end(); ++iter ) {

   		CommCreateEvent* ev = iter->second;
        for ( unsigned int i = 0; i < ev->members[0].size(); i++ ) {
            DBGX("%s\n", ev->members[0][i].c_str() );
            CommDestroyEvent* d_ev = new CommDestroyEvent;
            d_ev->commID = ev->commID;
            m_rtr.sendEvent( ev->members[0][i].c_str(), d_ev );
            delete d_ev;
        }

        delete ev;
   }
}

void Router::Client::addComm( CommID id, CommCreateEvent* ev ) {
	assert( m_commMap.find( id ) == m_commMap.end() );
    m_commMap[id] = ev;
}

std::vector< std::vector< ObjID > >& Router::Client::getCommList( CommID id ) {
	return  m_commMap[id]->members;
}
