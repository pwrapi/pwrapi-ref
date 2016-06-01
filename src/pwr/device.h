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

#ifndef _DEVICE_H
#define _DEVICE_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <vector>
#include <assert.h>
#include "pwrdev.h"
#include "debug.h"

namespace PowerAPI {

static inline int opAvg( int num, double input[], double* result, PWR_Time* ts)
{
    DBG("\n");
    double total = 0;
    for ( int i = 0; i < num; i++ ) {
        DBG("%f\n",input[i]);
        total += input[i];
    }
    *result = total/num;
    *ts = 0;
    return PWR_RET_SUCCESS;
}

class Device {

  public:
	Device( plugin_devops_t* ops, const std::string config )
      :  m_ops( ops )
    {
        DBGX("config=`%s`\n",config.c_str());
        m_fd = m_ops->open( ops, config.c_str() );
		assert( m_fd );
    }

    virtual ~Device() {
		m_ops->close( m_fd );
    }

	virtual int getValues( const std::vector<PWR_AttrName>& names, void* ptr,
                    std::vector<PWR_Time>& ts, std::vector<int>& status ){
        DBGX("\n");
        return m_ops->readv( m_fd, names.size(), &names[0], ptr,
                            &ts[0], &status[0] );
    }

    virtual int setValues( const std::vector<PWR_AttrName>& names, void* ptr,
                    std::vector<int>& status ){
        DBGX("\n");
        return m_ops->writev( m_fd, names.size(), &names[0], ptr,
                                                            &status[0] );
    }

    virtual int getValue( PWR_AttrName name, void* ptr, size_t len,
														PWR_Time* ts ){
        DBGX("\n");
        return m_ops->read( m_fd, name, ptr, len, ts );
    }

    virtual int setValue( PWR_AttrName name, void* ptr, size_t len ) {
        DBGX("\n");
        return m_ops->write( m_fd, name, ptr, len );
    }

    virtual int startLog( PWR_AttrName name ) {
        DBGX("\n");
        if ( m_ops->log_start ) {
            return m_ops->log_start( m_fd, name );
        } else {
            return PWR_RET_FAILURE;
        }
    }

    virtual int stopLog( PWR_AttrName name ) {
        DBGX("\n");
        if ( m_ops->log_stop ) {
            return m_ops->log_stop( m_fd, name );
        } else {
            return PWR_RET_FAILURE;
        }
    }

    virtual int getSamples( PWR_AttrName name, PWR_Time* ts, 
						double period, unsigned int* nSamples, void* results ) {
        DBGX("\n");
        if ( m_ops->get_samples ) {
            return m_ops->get_samples( m_fd, name, ts, period, nSamples, results );
        } else {
            return PWR_RET_FAILURE;
        }
    }

  private:
    plugin_devops_t*	m_ops;
    pwr_fd_t        	m_fd;	
};

}

#endif
