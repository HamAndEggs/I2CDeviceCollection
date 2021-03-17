#include "tmp102.h"

namespace i2c{
//////////////////////////////////////////////////////////////////////////
TMP102::TMP102(int pAddress,int pBus):Device(pAddress,pBus)
{
}

TMP102::~TMP102()
{
}

float TMP102::ReadTemp()const
{
	int32_t data = (int32_t)ReadWordData(0);
	// Correct to little endian and sign extend 12bit signed int to 16 bit.
	int16_t v = ((data&0x80)<<8) | ((data&0x80)<<7) | ((data&0x80)<<6) | ((data&0x80)<<5) | ((data&0xff)<<4) | ((data&0xf000)>>12);

	return ((float)v) * 0.0625;
}

//////////////////////////////////////////////////////////////////////////
};//	namespace i2c{
