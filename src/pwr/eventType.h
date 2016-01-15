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

#ifndef _EVENT_TYPE_H
#define _EVENT_TYPE_H

#define FOREACH_ENUM(NAME) \
    NAME(NotSet) \
    NAME(CommCreate) \
    NAME(CommDestroy) \
    NAME(CommReq) \
    NAME(CommResp) \
    NAME(CommLogReq) \
    NAME(CommLogResp) \
    NAME(CommGetSamplesReq) \
    NAME(CommGetSamplesResp) \
    NAME(ServerConnect) \
    NAME(Router2Router) \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum EventType {
	FOREACH_ENUM(GENERATE_ENUM)
};

#endif
