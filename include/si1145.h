#ifndef __SI1145_H__
#define __SI1145_H__

#include "i2c_device.h"

namespace i2c{
//////////////////////////////////////////////////////////////////////////
// Adafruit break out was used and their source was heavly referenced for this class.
// https://learn.adafruit.com/adafruit-si1145-breakout-board-uv-ir-visible-sensor?view=all
//////////////////////////////////////////////////////////////////////////
class SI1145 : private Device
{
public:
	SI1145(int pAddress,int pBus = 1);
	virtual ~SI1145();

	/*
    * Test if the device opened ok
    */
    bool IsOpen()const{return Device::IsOpen();}

	int ReadUV()const;
	int ReadIR()const;
	int ReadVisible()const;
	int ReadProx()const;

private:
	uint8_t WriteParam(uint8_t p, uint8_t v);
};

//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{


#endif //__SI1145_H__
