

#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <i2c/smbus.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <assert.h>
#include <iostream>

#include "i2c_device.h"

namespace i2c{
//////////////////////////////////////////////////////////////////////////
Device::Device(int pAddress,int pBus) : mFile(0)
{
	const std::string fname(std::string("/dev/i2c-")+std::to_string(pBus));
	mFile = open(fname.c_str(),O_RDWR);
	if( mFile > 0 )
	{
		if (ioctl(mFile, I2C_SLAVE, pAddress) == 0)
		{
			std::cout << "i2c device open 0x" << std::hex << pAddress << std::dec << std::endl;
		}
		else
		{
			Close();
		}
	}
}

Device::~Device()
{
	Close();
}

uint8_t Device::ReadByte()const
{
	if( IsOpen() )
		return (uint8_t)i2c_smbus_read_byte(mFile);
	return -1;
}

uint8_t Device::ReadByteData(uint8_t pCommand)const
{
	if( IsOpen() )
		return (uint8_t)i2c_smbus_read_byte_data(mFile,pCommand);
	return -1;
}

uint16_t Device::ReadWordData(uint8_t pCommand)const
{
	if( IsOpen() )
		return (uint16_t)i2c_smbus_read_word_data(mFile,pCommand);
	return -1;
}

int32_t Device::ReadData(uint8_t pCommand,uint8_t* pData,int32_t pDataSize)const
{
	assert(pDataSize <= 32);
	if( IsOpen() )
	{
		int32_t ret = i2c_smbus_read_i2c_block_data(mFile,pCommand,pDataSize,pData);
		if( ret > 0 )
			return ret;
	}
	return -1;
}

int32_t Device::WriteByte(uint8_t pValue)const
{
	if( IsOpen() )
		return (int32_t)i2c_smbus_write_byte(mFile,pValue);
	return -1;
}

int32_t Device::WriteByteData(uint8_t pCommand,uint8_t pValue)const
{
	if( IsOpen() )
		return (int32_t)i2c_smbus_write_byte_data(mFile,pCommand,pValue);
	return -1;
}

int32_t Device::WriteWordData(uint8_t pCommand,uint16_t pValue)const
{
	if( IsOpen() )
		return (int32_t)i2c_smbus_write_word_data(mFile,pCommand,pValue);
	return -1;
}

int32_t Device::WriteData(uint8_t pCommand,const uint8_t* pData,int32_t pDataSize)const
{
	assert(pDataSize <= 32);
	if( IsOpen() )
		return (int32_t)i2c_smbus_write_i2c_block_data(mFile,pCommand,(uint8_t)pDataSize,pData);
	return -1;
}

void Device::Close()
{
	if( mFile > 0 )
	{
		close(mFile);
		mFile = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
};//namespace i2c{
