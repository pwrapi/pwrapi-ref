
#ifndef _WORK_H
#define _WORK_H

#include <stdio.h>
#include <pwr.h>

namespace PWR_Logger {

class Work {
  public:
    virtual int work( FILE* ) = 0;
};

}

#endif
