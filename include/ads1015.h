#ifndef __I2C_ADS1015_H__
#define __I2C_ADS1015_H__

#include "i2c_device.h"

namespace i2c{
//////////////////////////////////////////////////////////////////////////
/*=========================================================================*/
// Lots of bits of code stolen from the adafruit Arduino library.
// Only put in what I need, nothing more.
// https://github.com/adafruit/Adafruit_ADS1X15
class ADS1015 : private Device
{
public:

    enum eGain
    {
        GAIN_TWOTHIRDS,
        GAIN_ONE,
        GAIN_TWO,
        GAIN_FOUR,
        GAIN_EIGHT,
        GAIN_SIXTEEN
    };


	ADS1015(int pAddress,int pBus = 1);
	virtual ~ADS1015();

    // The sensor reads negative and positive voltages.
    // It is a 12bit sensor so that is -2048 -> 2047
    int Read(int pChannel,eGain pGain = GAIN_TWO);

};

//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{

#endif /*__I2C_ADS1015_H__*/
