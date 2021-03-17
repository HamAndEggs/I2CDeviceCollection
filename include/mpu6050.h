#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "i2c_device.h"

namespace i2c{
//////////////////////////////////////////////////////////////////////////
// https://www.invensense.com/products/motion-tracking/6-axis/mpu-6050/

class MPU6050 : private Device
{
public:
	MPU6050(int pAddress,int pBus = 1);
	virtual ~MPU6050();

	/*
    * Test if the device opened ok
    */
    bool IsOpen()const{return Device::IsOpen();}

	bool ReadAccel(float& rX, float& rY, float& rZ);
	bool ReadGyro(float& rX, float& rY, float& rZ);

private:
};

//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{

#endif //__MPU6050_H__
