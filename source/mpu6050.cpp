#include <assert.h>
#include <iostream>
#include <thread>

#include "mpu6050.h"

const uint8_t SELF_TEST_X		= 0x0D;
const uint8_t SELF_TEST_Y		= 0x0E;
const uint8_t SELF_TEST_Z		= 0x0F;
const uint8_t SELF_TEST_A		= 0x10;
const uint8_t SMPLRT_DIV		= 0x19;
const uint8_t CONFIG			= 0x1A;
const uint8_t GYRO_CONFIG		= 0x1B;
const uint8_t ACCEL_CONFIG		= 0x1C;
const uint8_t FIFO_EN			= 0x23;
const uint8_t I2C_MST_CTRL 		= 0x24;
const uint8_t I2C_SLV0_ADDR		= 0x25;
const uint8_t I2C_SLV0_REG 		= 0x26;
const uint8_t I2C_SLV0_CTRL		= 0x27;
const uint8_t I2C_SLV1_ADDR		= 0x28;
const uint8_t I2C_SLV1_REG 		= 0x29;
const uint8_t I2C_SLV1_CTRL		= 0x2A;
const uint8_t I2C_SLV2_ADDR		= 0x2B;
const uint8_t I2C_SLV2_REG 		= 0x2C;
const uint8_t I2C_SLV2_CTRL		= 0x2D;
const uint8_t I2C_SLV3_ADDR		= 0x2E;
const uint8_t I2C_SLV3_REG 		= 0x2F;
const uint8_t I2C_SLV3_CTRL		= 0x30;
const uint8_t I2C_SLV4_ADDR		= 0x31;
const uint8_t I2C_SLV4_REG 		= 0x32;
const uint8_t I2C_SLV4_DO		= 0x33;
const uint8_t I2C_SLV4_CTRL		= 0x34;
const uint8_t I2C_SLV4_DI		= 0x35;
const uint8_t I2C_MST_STATU		= 0x36;
const uint8_t INT_PIN_CFG		= 0x37;
const uint8_t INT_ENABLE 		= 0x38;
const uint8_t INT_STATUS		= 0x3A;
const uint8_t ACCEL_XOUT_H		= 0x3B;
const uint8_t ACCEL_XOUT_L		= 0x3C;
const uint8_t ACCEL_YOUT_H		= 0x3D;
const uint8_t ACCEL_YOUT_L		= 0x3E;
const uint8_t ACCEL_ZOUT_H		= 0x3F;
const uint8_t ACCEL_ZOUT_L		= 0x40;
const uint8_t TEMP_OUT_H		= 0x41;
const uint8_t TEMP_OUT_L		= 0x42;
const uint8_t GYRO_XOUT_H		= 0x43;
const uint8_t GYRO_XOUT_L		= 0x44;
const uint8_t GYRO_YOUT_H		= 0x45;
const uint8_t GYRO_YOUT_L		= 0x46;
const uint8_t GYRO_ZOUT_H		= 0x47;
const uint8_t GYRO_ZOUT_L		= 0x48;
const uint8_t EXT_SENS_DATA_00	= 0x49;
const uint8_t EXT_SENS_DATA_01	= 0x4A;
const uint8_t EXT_SENS_DATA_02	= 0x4B;
const uint8_t EXT_SENS_DATA_03	= 0x4C;
const uint8_t EXT_SENS_DATA_04	= 0x4D;
const uint8_t EXT_SENS_DATA_05	= 0x4E;
const uint8_t EXT_SENS_DATA_06	= 0x4F;
const uint8_t EXT_SENS_DATA_07	= 0x50;
const uint8_t EXT_SENS_DATA_08	= 0x51;
const uint8_t EXT_SENS_DATA_09	= 0x52;
const uint8_t EXT_SENS_DATA_10	= 0x53;
const uint8_t EXT_SENS_DATA_11	= 0x54;
const uint8_t EXT_SENS_DATA_12	= 0x55;
const uint8_t EXT_SENS_DATA_13	= 0x56;
const uint8_t EXT_SENS_DATA_14	= 0x57;
const uint8_t EXT_SENS_DATA_15	= 0x58;
const uint8_t EXT_SENS_DATA_16	= 0x59;
const uint8_t EXT_SENS_DATA_17	= 0x5A;
const uint8_t EXT_SENS_DATA_18	= 0x5B;
const uint8_t EXT_SENS_DATA_19	= 0x5C;
const uint8_t EXT_SENS_DATA_20	= 0x5D;
const uint8_t EXT_SENS_DATA_21	= 0x5E;
const uint8_t EXT_SENS_DATA_22	= 0x5F;
const uint8_t EXT_SENS_DATA_23	= 0x60;
const uint8_t I2C_SLV0_DO		= 0x63;
const uint8_t I2C_SLV1_DO		= 0x64;
const uint8_t I2C_SLV2_DO		= 0x65;
const uint8_t I2C_SLV3_DO		= 0x66;
const uint8_t I2C_MST_DELAY_CT	= 0x67;
const uint8_t SIGNAL_PATH_RES	= 0x68;
const uint8_t USER_CTRL			= 0x6A;
const uint8_t PWR_MGMT_1		= 0x6B;
const uint8_t PWR_MGMT_2		= 0x6C;
const uint8_t FIFO_COUNTH		= 0x72;
const uint8_t FIFO_COUNTL		= 0x73;
const uint8_t FIFO_R_W			= 0x74;
const uint8_t WHO_AM_I			= 0x75;



namespace i2c{
//////////////////////////////////////////////////////////////////////////
MPU6050::MPU6050(int pAddress,int pBus):Device(pAddress,pBus)
{
	if( IsOpen() )
	{
	}
}

MPU6050::~MPU6050()
{
}


//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{
