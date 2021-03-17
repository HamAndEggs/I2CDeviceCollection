#include <chrono>
#include <thread>
#include <iostream>
#include <math.h>
#include "pca9685.h"

#define MODE1 0x00			//Mode  register  1
#define MODE2 0x01			//Mode  register  2
#define SUBADR1 0x02		//I2C-bus subaddress 1
#define SUBADR2 0x03		//I2C-bus subaddress 2
#define SUBADR3 0x04		//I2C-bus subaddress 3
#define ALLCALLADR 0x05     //LED All Call I2C-bus address
#define LED0 0x6			//LED0 start register
#define LED0_ON_L 0x6		//LED0 output and brightness control byte 0
#define LED0_ON_H 0x7		//LED0 output and brightness control byte 1
#define LED0_OFF_L 0x8		//LED0 output and brightness control byte 2
#define LED0_OFF_H 0x9		//LED0 output and brightness control byte 3
#define LED_MULTIPLYER 4	// For the other 15 channels
#define ALLLED_ON_L 0xFA    //load all the LEDn_ON registers, byte 0 (turn 0-7 channels on)
#define ALLLED_ON_H 0xFB	//load all the LEDn_ON registers, byte 1 (turn 8-15 channels on)
#define ALLLED_OFF_L 0xFC	//load all the LEDn_OFF registers, byte 0 (turn 0-7 channels off)
#define ALLLED_OFF_H 0xFD	//load all the LEDn_OFF registers, byte 1 (turn 8-15 channels off)
#define PRE_SCALE 0xFE		//prescaler for output frequency

namespace i2c
{
//////////////////////////////////////////////////////////////////////////
PCA9685::PCA9685(int pAddress,int pBus):Device(pAddress,pBus)
{
}

PCA9685::~PCA9685()
{
	// Turn off the pwm.
	SetEnabled(false);
}

void PCA9685::SetEnabled(bool pEnable)
{
	if( pEnable )
	{
        WriteByteData(MODE1, 0); //restart
	}
	else
	{
		WriteByteData(MODE1, 0x10); //sleep
	}
	std::this_thread::sleep_for(std::chrono::microseconds(500));
}

void PCA9685::SetPrescale(uint8_t pPrescale)
{
    if( IsOpen() )
    {
        WriteByteData(MODE1, 0x10); //sleep
        WriteByteData(PRE_SCALE, pPrescale); // multiplyer for PWM frequency
        // See docs 7.3.1.1 Restart mode.
		std::this_thread::sleep_for(std::chrono::microseconds(500));
        WriteByteData(MODE1, 0x80); //restart
        WriteByteData(MODE2, 0x04);
    }
}

void PCA9685::SetPWM(int pPin,int pOnTime,int pOffTime)
{
    if( IsOpen() )
    {
        WriteByteData(LED0_ON_L  + LED_MULTIPLYER * pPin, pOnTime & 0xFF);
        WriteByteData(LED0_ON_H  + LED_MULTIPLYER * pPin, pOnTime >> 8);
        WriteByteData(LED0_OFF_L + LED_MULTIPLYER * pPin, pOffTime & 0xFF);
        WriteByteData(LED0_OFF_H + LED_MULTIPLYER * pPin, pOffTime >> 8);
    }
}


void PCA9685::SetFrequency(int pFrequency,int pErrorAdjust)
{
    const int CLOCK_FREQ = 25000000; //25MHz default osc clock
    const uint8_t prescale_val = uint8_t(CLOCK_FREQ / (4096 * pFrequency)) + pErrorAdjust;				
    SetPrescale(prescale_val);
}

void PCA9685::SetPWMMicroseconds(int pPin,int pMicroseconds,int pFrequency)
{
    const int TotalMicroseconds = 1000000 / pFrequency;   // How long each cycle is in microseconds.

    // Now to get into 0 -> 4096 or our frequency.
    const int OffTime = pMicroseconds * 4096 / TotalMicroseconds;

    SetPWM(pPin,OffTime);
}


//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{
