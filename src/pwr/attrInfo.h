#ifndef _ATTR_INFO_H
#define _ATTR_INFO_H

#include <deque>

namespace PowerAPI {

class Device; 
class Communicator;

class AttrInfo {
  public:
	typedef void (*OpFuncPtr)( void*, void* );
	typedef PWR_Time (*TimeFuncPtr)( PWR_Time, PWR_Time );

	AttrInfo( OpFuncPtr fptr, TimeFuncPtr tptr) : 
		comm( NULL ), operation(fptr), calcTime(tptr) {}

	virtual bool isValid() { 
		return ! devices.empty() || comm; 
	}   

	std::deque<Device*> devices;
	Communicator*		comm;
	OpFuncPtr			operation;
	TimeFuncPtr 		calcTime;
};

}

#endif
