#ifndef __I2C_BMP280_H__
#define __I2C_BMP280_H__

#include <stdint.h>
#include "i2c_device.h"

namespace i2c{
//////////////////////////////////////////////////////////////////////////
// This code follows the bosch example and puts the device in low power mode
// once the data has been read.
// Their driver code is included in this lib but not used as it does not allow me to have more
// than one object of this class. And I don't want to limit the code to this.
// So like most I have copied and pasted code from their 'driver' into mine.
class BMP280 : private Device
{
public:
	BMP280(int pAddress,int pBus = 1);
	virtual ~BMP280();

	/*
    * Test if the device opened ok
    */
    bool IsOpen()const{return Device::IsOpen();}

    // Does the atchal read, use before calling GetTemp and GetPressure. 
    // Done like this as they both depend on the same data.
    // Returns true if data read ok.
    bool Read();    

	float GetTemp()const{return mTemp;}
	float GetPressure()const{return mPressure;}

private:

    bool SetWorkModePowerMode(uint8_t pWorkMode,uint8_t pPowerMode);
    bool SetStandbyMode(uint8_t pMode);
    bool ReadRawData(int32_t& rPressureRaw, int32_t& rTemperatureRaw);

    uint8_t mChipId;
    float mTemp;
    float mPressure;
    struct
    {
        uint16_t dig_T1;/**<calibration T1 data*/
        int16_t dig_T2;/**<calibration T2 data*/
        int16_t dig_T3;/**<calibration T3 data*/
        uint16_t dig_P1;/**<calibration P1 data*/
        int16_t dig_P2;/**<calibration P2 data*/
        int16_t dig_P3;/**<calibration P3 data*/
        int16_t dig_P4;/**<calibration P4 data*/
        int16_t dig_P5;/**<calibration P5 data*/
        int16_t dig_P6;/**<calibration P6 data*/
        int16_t dig_P7;/**<calibration P7 data*/
        int16_t dig_P8;/**<calibration P8 data*/
        int16_t dig_P9;/**<calibration P9 data*/

        int32_t t_fine;/**<calibration t_fine data*/
    }mCalibrationParameters;

};

//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{

#endif /*__I2C_BMP280_H__*/
