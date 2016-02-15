
#ifndef _TIME_UTIL_H
#define  _TIME_UTIL_H

#include <sys/time.h>

static inline double getTime() {
    struct timeval now;
    gettimeofday(&now, NULL);

    return (now.tv_sec * 1000) + ((double) now.tv_usec / 1000.0);
}

#endif
