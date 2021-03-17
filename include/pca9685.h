#ifndef __PCA9685_H__
#define __PCA9685_H__

#include "i2c_device.h"

namespace i2c
{
//////////////////////////////////////////////////////////////////////////
// Referenced the adafruit arduino driver.
// https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
// Also read code from TeraHz.
// https://github.com/TeraHz/PCA9685/blob/master/src/PCA9685.cpp
//
// Datasheet
// http://www.nxp.com/documents/data_sheet/PCA9685.pdf
//////////////////////////////////////////////////////////////////////////
class PCA9685 : private Device
{
public:
	PCA9685(int pAddress,int pBus = 1);
	virtual ~PCA9685();

    /*
    * Test if the device opened ok
    */
    bool IsOpen()const{return Device::IsOpen();}

	/*
		Turn all PWM on and OFF.
	*/
	void SetEnabled(bool pEnable);

    /*
    * Sets the hardware pre scale. The hardware does not allow a value less than 3.
    * As it's 8 bit, values > 255 can't be used either.
    * I have found some variation from what the datasheet says, and so you should scope
    * your results to make sure you're getting what you want.
    * A value of 121 should give 50Hz, for mine, 130 was required to get 50Hz. I was seing a 10% error.
    */
    void SetPrescale(uint8_t pPrescale);

    /*
    * Sets the time on and off with the range of 0 -> 4095.
    * The time that it is on and off by is a function of the frequency.
    */
    void SetPWM(int pPin,int pOnTime,int pOffTime);//pOnTime and pOffTime 0 -> 4095.
    void SetPWM(int pPin,int pValue){SetPWM(pPin,0,pValue);}//pValue 0 -> 4095.

    /*
    * Utility function used to set the prescale from a human readable value.
    * pErrorAdjust is to allow compensation for hardware differences.
    */
    void SetFrequency(int pFrequency,int pErrorAdjust = -1);//pErrorAdjust is because there seems to be some varibilty in the chips. -1 is what the datasheet uses. I have had to use 8 for my chip.

    /*
    * Utility function used to set the off time based on microseconds, used for servos.
    * On time is always set to 0.
    * pFrequency is the update cycle you set in Hertz. Not the prescale value. It's used in the math.
    */
    void SetPWMMicroseconds(int pPin,int pMicroseconds,int pFrequency);
};

//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{

#endif //__PCA9685_H__
