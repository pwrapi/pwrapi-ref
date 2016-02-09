
#ifndef _WORK_H
#define _WORK_H

#include <stdio.h>
#include <pwr.h>

namespace PWR_Logger {

class Work {
  public:
    virtual int work( PWR_Cntxt, PWR_Obj, FILE* ) = 0;
};

}

#endif
