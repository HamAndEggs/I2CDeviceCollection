#ifndef __TLS2561_H__
#define __TLS2561_H__

#include <chrono>

#include "i2c_device.h"

namespace i2c{
//////////////////////////////////////////////////////////////////////////
// Based on the datasheat and the adafruit library.
// http://ams.com/eng/Products/Light-Sensors/Ambient-Light-Sensors/TSL2561
// http://ams.com/eng/Products/Light-Sensors/Ambient-Light-Sensors/TSL2560/TSL2560-TSL2561-Datasheet
// https://github.com/adafruit/TSL2561-Arduino-Library
class TSL2561 : private Device
{
public:
	enum eIntergrationTime
	{
		INTERGRATION_TIME_13MS,
		INTERGRATION_TIME_101MS,
		INTERGRATION_TIME_401MS,	// Default value.
	};

	TSL2561(int pAddress,int pBus = 1,bool pPackageCS = false); //Adafruit uses FN package. So default to false.
	virtual ~TSL2561();

	/*
    * Test if the device opened ok
    */
    bool IsOpen()const{return Device::IsOpen();}

	void SetIntergrationTime(int pTime);
	void SetEnableGain(bool pEnable);	// Turns on 16x gain.

	// Will wait between n milliseconds before waking the device up and taking a reading.
	// The intergration time will dictate when the readings are taken after the device was woken up.
	// Done like this so that the read is not blocking and is not on a second thread.
	// If this is not periodically called then readings will not be taken.
	void Read(int pMilliseconds); // pMilliseconds how long between each reading. Values less than intergration time are ignored.

	// Returns the last values read.
	int GetIR()const{return mIR;}
	int GetLux()const{return mLux;}

private:
	const bool mPackageCS;	// See datasheet pages 45 and 46. Adafruit uses FN package. So set to false.
	int mIntergrationTime;
	bool mEnableGain;		// On by default.
	int mIR;
	int mLux;
	std::chrono::milliseconds mNextRead;	// Based on the intergration time set. When this time passes will update the IR and LUX values.
	std::chrono::milliseconds mReadingReady;	// When the reading will be ready.

	int CalculateLux(uint32_t pChannel0,uint32_t pChannel1)const;
	void UpdateRegisterTiming();
};

//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{

#endif //__TLS2561_H__
