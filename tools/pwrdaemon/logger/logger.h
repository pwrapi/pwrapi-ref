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

#ifndef _LOGGER_H
#define _LOGGER_H

#include <string>
#include <pwr.h>


namespace PWR_Logger {

struct Args {
    std::string port;
    std::string host;

    std::string pwrApiConfig;
    std::string pwrApiRoot;
    std::string pwrApiServer;
    std::string pwrApiServerPort;
    std::string objectName;
    std::string ctxName;
    std::string logfile;
    std::string count;
    std::string delay;
    std::string attr;
};

class Work;

class Logger {
  public:
	Logger( int, char* [] );
	~Logger();
	int work();

  private:
	Work*     	m_work;
	Args		m_args;
	PWR_Cntxt 	m_ctx;
	FILE*		m_logFP;
	int     	m_delay;
};

}

#endif
