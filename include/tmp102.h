#ifndef __TMP102_H__
#define __TMP102_H__

#include "i2c_device.h"

namespace i2c{
//////////////////////////////////////////////////////////////////////////
class TMP102 : private Device
{
public:
	TMP102(int pAddress,int pBus = 1);
	virtual ~TMP102();

	/*
    * Test if the device opened ok
    */
    bool IsOpen()const{return Device::IsOpen();}

	float ReadTemp()const;

private:

};

//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{

#endif //__TMP102_H__
