#ifndef _ATTR_INFO_H
#define _ATTR_INFO_H

#include <deque>
#include <communicator.h>

namespace PowerAPI {

class Device; 
class Communicator;

class AttrInfo {
  public:
	typedef void (*OpFuncPtr)( void* out, void* in, size_t num );
	typedef PWR_Time (*TimeFuncPtr)( std::vector<PWR_Time> );

	AttrInfo( OpFuncPtr fptr, TimeFuncPtr tptr, ValueOp op) : 
		comm( NULL ), operation(fptr), calcTime(tptr), valueOp(op) {}
	virtual ~AttrInfo() {}

	virtual bool isValid() { 
		return ! devices.empty() || comm; 
	}   

	std::vector<Device*> devices;
	Communicator*		comm;
	OpFuncPtr			operation;
	TimeFuncPtr 		calcTime;
	ValueOp				valueOp;	
};

}

#endif
