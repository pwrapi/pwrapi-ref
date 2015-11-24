
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
